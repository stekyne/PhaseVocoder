#pragma once

#include <vector>
#include <algorithm>
#include "PhaseVocoder.h"

class PitchShifter
{
public:
	PitchShifter () :
		synthPhaseIncrements (phaseVocoder.getWindowSize(), 0),
		previousFramePhases (phaseVocoder.getWindowSize(), 0)
	{
		setPitchRatio (1.f);
	}

	int getLatencyInSamples() const { return phaseVocoder.getLatencyInSamples(); }

	void setPitchRatio (float newPitchRatio)
	{
		if (phaseVocoder.getPitchRatio() == newPitchRatio) 
			return;

		const juce::SpinLock::ScopedLockType lock(phaseVocoder.getParamLock());

		phaseVocoder.setPitchRatio(newPitchRatio);
		phaseVocoder.setSynthesisHopSize((int)(phaseVocoder.getWindowSize() / (float)phaseVocoder.getWindowOverlapCount()));
		phaseVocoder.setAnalysisHopSize((int)round(phaseVocoder.getSynthesisHopSize() / phaseVocoder.getPitchRatio()));

		// Rescaling due to OLA processing gain
		double accum = 0.0;
		auto windowFunction = phaseVocoder.getWindowFunction();

		for (int i = 0; i < phaseVocoder.getWindowSize(); ++i)
			accum += windowFunction[i] * (double)windowFunction[i];

		accum /= phaseVocoder.getSynthesisHopSize();
		phaseVocoder.setRescalingFactor((float)accum);
		phaseVocoder.updateResampleBufferSize();
		phaseVocoder.setSynthesisHopSize(phaseVocoder.getAnalysisHopSize());

		// Reset phases
		memset(previousFramePhases.data(), 0, sizeof(FloatType) * phaseVocoder.getWindowSize());
		memset(synthPhaseIncrements.data(), 0, sizeof(FloatType) * phaseVocoder.getWindowSize());
	}

	void process(FloatType* const buffer, const int bufferSize)
	{
		phaseVocoder.process(buffer, bufferSize, [&](FloatType* const buffer, const int bufferSize)
		{
			for (int i = 0, x = 0; i < bufferSize - 1; i += 2, ++x)
			{
				// Update phase increments for pitch shifting
				const auto real = buffer[i];
				const auto imag = buffer[i + 1];
				const auto mag = sqrtf(real * real + imag * imag);
				const auto phase = atan2(imag, real);

				const auto omega = juce::MathConstants<float>::twoPi * phaseVocoder.getAnalysisHopSize() *
					x / (float)phaseVocoder.getWindowSize();

				const auto deltaPhase = omega + PhaseVocoder::principalArgument(
					phase - previousFramePhases[x] - omega);

				previousFramePhases[x] = phase;
				synthPhaseIncrements[x] = PhaseVocoder::principalArgument(synthPhaseIncrements[x] +
					(deltaPhase * phaseVocoder.getTimeStretchRatio()));

				buffer[i] = mag * std::cos(synthPhaseIncrements[x]);
				buffer[i + 1] = mag * std::sin(synthPhaseIncrements[x]);
			}
		});
	}

private:
	PhaseVocoder phaseVocoder;
	std::vector<FloatType> synthPhaseIncrements;
	std::vector<FloatType> previousFramePhases;
};
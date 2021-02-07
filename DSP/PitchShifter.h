#pragma once

#include <vector>
#include <algorithm>
#include "PhaseVocoder.h"

template<typename FloatType = float>
class PitchShifter : public PhaseVocoder<FloatType>
{
public:
	PitchShifter () :
		synthPhaseIncrements (this->windowSize, 0),
		previousFramePhases (this->windowSize, 0)
	{
		
		setPitchRatio (1.f);
	}

	void setPitchRatio (float ratio)
	{
		if (pitchRatio == ratio) 
			return;

		// Lower ratios require a larger amount of incoming samples
		// This will introduce more latency and large analysis and synthesis buffers
		pitchRatio = std::clamp (ratio, MinPitchRatio, MaxPitchRatio);
		
		synthesisHopSize = (int)(windowSize / (float)MinOverlapAmount);
		analysisHopSize = (int)round (synthesisHopSize / pitchRatio);
		resampleSize = (int)std::ceil (windowSize * analysisHopSize / (float)synthesisHopSize);
		timeStretchRatio = synthesisHopSize / (float)analysisHopSize;

		// Rescaling due to OLA processing gain
		double accum = 0.0;

		for (int i = 0; i < windowSize; ++i)
			accum += windowBuffer[i] * (double)windowBuffer[i];

		accum /= synthesisHopSize;
		rescalingFactor = (float)accum;
		synthesisHopSize = analysisHopSize;

		// TODO do we need to reset store phase values?
	}

	void processImpl (FloatType* const buffer, const int bufferSize) override final
	{
		// Update phase increments for pitch shifting
		for (int i = 0, x = 0; i < bufferSize - 1; i += 2, ++x)
		{
			const auto real = buffer[i];
			const auto imag = buffer[i + 1];
			const auto mag = sqrtf (real * real + imag * imag);
			const auto phase = atan2 (imag, real);
			const auto omega = juce::MathConstants<float>::twoPi * analysisHopSize * x / (float)windowSize;

			const auto deltaPhase = omega + principalArgument (
				phase - previousFramePhases[x] - omega);

			previousFramePhases[x] = phase;

			synthPhaseIncrements[x] = principalArgument (synthPhaseIncrements[x] +
				(deltaPhase * timeStretchRatio));

			buffer[i] = mag * cos (synthPhaseIncrements[x]);
			buffer[i + 1] = mag * sin (synthPhaseIncrements[x]);
		}
	}

private:
	std::vector<FloatType> synthPhaseIncrements;
	std::vector<FloatType> previousFramePhases;
	
	float pitchRatio = 0.f;
	float timeStretchRatio = 1.f;
};
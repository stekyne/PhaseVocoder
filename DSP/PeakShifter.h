#pragma once

#include "PhaseVocoder.h"

template<typename FloatType = float>
class PeakShifter : public PhaseVocoder<FloatType>
{
public:
	PeakShifter () :
		phi0 (windowSize / 2, 0),
		psi (windowSize / 2, 0),
		psi2 (windowSize / 2, 0),
		peaks (windowSize / 2, 0)
	{
	}

	void setPitchRatio (float ratio)
	{
		if (pitchRatio == ratio)
			return;

		pitchRatio = std::clamp (ratio, MinPitchRatio, MaxPitchRatio);;
		timeStretchRatio = synthesisHopSize / (float)analysisHopSize;
	}

	void processImpl (FloatType* const buffer, const int bufferSize) override final
	{
		auto npeaks = 0;
		std::vector<FloatType> mags (bufferSize, 0);
		std::vector<FloatType> phases (bufferSize, 0);

		for (int i = 0, x = 0; i < bufferSize - 1; i += 2, ++x)
		{
			const auto real = buffer[i];
			const auto imag = buffer[i + 1];
			mags[x] = sqrtf (real * real + imag * imag);
			phases[x] = atan2 (imag, real);
		}

		peaks.clear ();

		// Find spectral peaks (local maxima)
		for (int i = 3; i < (bufferSize / 2) - 2; i += 3)
		{
			if (mags[i] > mags[i - 1] && mags[i] > mags[i - 2] &&
				mags[i] > mags[i + 1] && mags[i] > mags[i + 2])
			{
				npeaks += 1;
				peaks.push_back (i);
			}
		}

		// Propagate peak phases and compute spectral bin phases
		if (psi.size () == 0)
		{
			psi = phases;
		}
		else if (npeaks > 0 && nprevpeaks > 0)
		{
			auto prev_p = 1;

			for (int p = 0; p < npeaks; ++p)
			{
				const auto peak = peaks[p];
				const auto prev_peak = prev_peaks[prev_p];

				// Connect current peak to the previous closest peak
				while (prev_p < nprevpeaks && 
					std::abs (peak - prev_peaks[prev_p + 1]) < std::abs (peak - prev_peak))
				{
					prev_p += 1;
				}

				// Propagate peak's phase assuming linear frequency
				// Variation between connected peaks p1 and p2
				const auto avg_p = (peak + prev_peak) * FloatType (0.5);
				const auto omega = juce::MathConstants<float>::twoPi * analysisHopSize * avg_p / (float)windowSize;
				const auto peak_delta_phi = omega + principalArgument (phases[peak] + phi0[prev_peak] - omega);
				const auto peak_target_phase = principalArgument (psi [prev_peak] + peak_delta_phi * timeStretchRatio);
				const auto peak_phase_rotation = principalArgument (peak_target_phase - phases[peak]);

				auto bin1 = 1;
				auto bin2 = windowSize / 2;

				// Rotate phases of all bins around the current peak
				if (p == npeaks)
				{
					bin1 = (int)std::round ((peaks[p - 1] + peak) * FloatType (0.5));
				}
				else if (npeaks != 1 && p != 1)
				{
					bin1 = (int)std::round ((peaks[p - 1] + peak) * FloatType (0.5));
					bin2 = (int)std::round ((peaks[p + 1] + peak) * FloatType (0.5));
				}

				for (auto i = 0; i < bin2 - bin1; ++i)
					psi2[bin1 + i] = principalArgument (phases[bin1 + i] + peak_phase_rotation);
			}

			psi = psi2;
		}
		else
		{
			for (auto i = 0; i < windowSize / 2; ++i)
			{
				const auto omega = juce::MathConstants<float>::twoPi * analysisHopSize * i / (float)windowSize;
				const auto deltaPhi = omega + principalArgument (phases[i] - phi0[i] - omega);
				psi[i] = principalArgument (psi[i] + deltaPhi * timeStretchRatio);
			}
		}

		// Store state
		phi0 = phases;
		prev_peaks = peaks;
		nprevpeaks = npeaks;

		// Reconstruct whole spectrum
		for (auto i = 0, x = 0; i < bufferSize - 1; i += 2, ++x)
		{
			buffer[i] = mags[x] * cos (psi[x]);
			buffer[i + 1] = mags[x] * sin (psi[x]);
		}

		// TODO conjugate first half of spectrum as per dafx example?
	}

private:
	std::vector<FloatType> phi0;
	std::vector<FloatType> psi;
	std::vector<FloatType> psi2;
	std::vector<int> peaks;
	std::vector<int> prev_peaks;
	float pitchRatio = 0.f;
	float timeStretchRatio = 1.f;
	int nprevpeaks = 0;
};
#pragma once

#include <juce_dsp/juce_dsp.h>
#include <algorithm>
#include <functional>
#include "BlockCircularBuffer.h"

// Resample a signal to a new size using linear interpolation
// The 'originalSize' is the max size of the original signal
// The 'newSignalSize' is the size to resample to. The 'newSignal' must be at least as big as this size.
static void linearResample (const float* const originalSignal, const int originalSize,
	float* const newSignal, const int newSignalSize)
{
	const auto lerp = [&](float v0, float v1, float t)
	{
		return (1.f - t) * v0 + t * v1;
	};

	// If the original signal is bigger than the new size, condense the signal to fit the new buffer
	// otherwise expand the signal to fit the new buffer
	const auto scale = originalSize / (float)newSignalSize;
	float index = 0.f;

	for (int i = 0; i < newSignalSize; ++i)
	{
		const auto wholeIndex = (int)floor (index);
		const auto fractionIndex = index - wholeIndex;
		const auto sampleA = originalSignal[wholeIndex];
		const auto sampleB = originalSignal[wholeIndex + 1];
		newSignal[i] = lerp (sampleA, sampleB, fractionIndex);
		index += scale;
	}
}

template <typename FloatType = float>
class PhaseVocoder
{
public: 
	static constexpr int MinOverlapAmount = 4;
	static constexpr float MaxPitchRatio = 2.0f;
	static constexpr float MinPitchRatio = 0.5f;

	enum Windows {
		hann,
		hamming,
		kaiser
	};

public:
	// Default settings
	PhaseVocoder (int windowLength = 2048, int fftSize = 2048, 
		Windows windowType = Windows::hann) :
		samplesTilNextProcess (windowLength),
		analysisHopSize (windowLength / MinOverlapAmount),
		synthesisHopSize (windowLength / MinOverlapAmount),
		windowSize (windowLength),
		resampleSize (windowLength),
		spectralBufferSize (windowLength * 2),
		analysisBuffer (windowLength),
		synthesisBuffer (windowLength * 3),
		windowBuffer(windowLength),
		fft(std::make_unique<juce::dsp::FFT>(nearestPower2(fftSize)))
	{
		initialiseWindow(getWindowForEnum(windowType));

		// Processing reuses the spectral buffer to resize the output grain
		// It must be the at least the size of the min pitch ratio
		// TODO FFT size must be big enough also
		spectralBufferSize = windowLength * (1 / MinPitchRatio) < spectralBufferSize ? 
			(int)ceil (windowLength * (1 / MinPitchRatio)) : spectralBufferSize;

		spectralBuffer.resize(spectralBufferSize);
		std::fill (spectralBuffer.data(), spectralBuffer.data() + spectralBufferSize, 0.f);

		// Calculate maximium size resample signal can be
		const auto maxResampleSize = (int)std::ceil (std::max (this->windowSize * MaxPitchRatio,
			this->windowSize / MinPitchRatio));

		resampleBuffer.resize(maxResampleSize);
		std::fill (resampleBuffer.data(), resampleBuffer.data() + maxResampleSize, 0.f);
	}

	~PhaseVocoder()
	{
	}
	
	int getLatencyInSamples () const
	{
		return windowSize;
	}

	// The main process function corresponds to the following high level algorithm
	// Note: The processing is split up internally to avoid extra memory usage
	// 1. Read incoming samples into the internal analysis buffer
	// 2. If there are enough samples to begin processing, read a block from the analysis buffer
	// 3. Perform an FFT of on the block of samples
	// 4. Do some processing with the spectral data
	// 5. Perform an iFFT back into the time domain
	// 6. Write the block of samples back into the internal synthesis buffer
	// 7. Read a block of samples from the synthesis buffer
	void process (FloatType* const incomingBuffer, const int incomingBufferSize)
	{
		juce::ScopedNoDenormals noDenormals;
		const juce::SpinLock::ScopedLockType lock(paramLock);

		static int callbackCount = 0;
		DBG (" ");
		DBG ("Callback: " << ++callbackCount << ", SampleCount: " << incomingSampleCount << 
			", (+ incoming): " << incomingBufferSize);

		// Only write enough samples into the analysis buffer to complete a processing
		// frame. Likewise, only write enough into the synthesis buffer to generate the 
		// next output audio frame. 
		for (auto internalOffset = 0, internalBufferSize = 0; 
			internalOffset < incomingBufferSize; 
			internalOffset += internalBufferSize)
		{
			const auto remainingIncomingSamples = (incomingBufferSize - internalOffset);
			internalBufferSize = incomingSampleCount + remainingIncomingSamples >= samplesTilNextProcess ?
				samplesTilNextProcess - incomingSampleCount : remainingIncomingSamples;
			
			DBG ("Internal buffer: Offset: " << internalOffset << ", Size: " << internalBufferSize);
			jassert (internalBufferSize <= incomingBufferSize);

			// Write the incoming samples into the internal buffer
			// Once there are enough samples, perform spectral processing
			const auto previousAnalysisWriteIndex = analysisBuffer.getReadIndex ();
			analysisBuffer.write (incomingBuffer + internalOffset, internalBufferSize);
			DBG ("Analysis Write Index: " << previousAnalysisWriteIndex << " -> " << analysisBuffer.getWriteIndex ());

			incomingSampleCount += internalBufferSize;

			// Collected enough samples, do processing
			if (incomingSampleCount >= samplesTilNextProcess)
			{
				isProcessing = true;
				incomingSampleCount -= samplesTilNextProcess;
				DBG (" ");
				DBG ("Process: SampleCount: " << incomingSampleCount);

				// After first processing, do another process every analysisHopSize samples
				samplesTilNextProcess = analysisHopSize;

				auto spectralBufferData = spectralBuffer.data();
				
				jassert (spectralBufferSize > windowSize);
				analysisBuffer.setReadHopSize (analysisHopSize);
				analysisBuffer.read (spectralBufferData, windowSize);
				DBG ("Analysis Read Index: " << analysisBuffer.getReadIndex ());

				// Apply window to signal
				juce::FloatVectorOperations::multiply (spectralBufferData, windowBuffer.data(), windowSize);

				// Rotate signal 180 degrees, move the first half to the back and back to the front
				std::rotate (spectralBufferData, spectralBufferData + (windowSize / 2), spectralBufferData + windowSize);
				
				// Perform FFT, process and inverse FFT
				fft->performRealOnlyForwardTransform (spectralBufferData);
				processImpl (spectralBufferData, spectralBufferSize);
				fft->performRealOnlyInverseTransform (spectralBufferData);

				// Undo signal back to original rotation
				std::rotate (spectralBufferData, spectralBufferData + (windowSize / 2), spectralBufferData + windowSize);

				// Apply window to signal
				juce::FloatVectorOperations::multiply (spectralBufferData, windowBuffer.data(), windowSize);

				// Resample output grain to N * (hop size analysis / hop size synthesis)
				linearResample (spectralBufferData, windowSize, resampleBuffer.data(), resampleSize);

				synthesisBuffer.setWriteHopSize (synthesisHopSize);
				synthesisBuffer.overlapWrite (resampleBuffer.data(), resampleSize);
				DBG ("Synthesis Write Index: " << synthesisBuffer.getWriteIndex ());
			}

			// Emit silence until we start producing output
			if (!isProcessing)
			{
				std::fill (incomingBuffer + internalOffset, incomingBuffer + internalOffset +
					internalBufferSize, 0.f);
				
				DBG ("Zeroed output: " << internalOffset << " -> " << internalBufferSize);
				continue;
			}

			const auto previousSynthesisReadIndex = synthesisBuffer.getReadIndex ();
			synthesisBuffer.read (incomingBuffer + internalOffset, internalBufferSize);
			DBG ("Synthesis Read Index: " << previousSynthesisReadIndex << " -> " << synthesisBuffer.getReadIndex ());
		}

		// Rescale output
		juce::FloatVectorOperations::multiply (incomingBuffer, 1.f / rescalingFactor, incomingBufferSize);
	}

	// Principal argument - Unwrap a phase argument to between [-PI, PI]
	static float principalArgument (float arg)
	{
		return std::fmod (arg + juce::MathConstants<FloatType>::pi, 
			-juce::MathConstants<FloatType>::twoPi) + juce::MathConstants<FloatType>::pi;
	}

	// Returns the 2^x exponent for a given power of two value
	// If the value given is not a power of two, the nearest power 2 will be used
	static int nearestPower2 (int value)
	{
		return (int)log2 (juce::nextPowerOfTwo (value));
	}

private:
	virtual void processImpl (FloatType* const, const int) = 0;

	using JuceWindowTypes = typename juce::dsp::WindowingFunction<FloatType>::WindowingMethod;

	int getOverlapsRequiredForWindowType(Windows windowType) const
	{
		switch (windowType) 
		{
		case Windows::hann:
		case Windows::hamming: 
				return 4;

		case Windows::kaiser: 
			return 8;

		default: 
			return -1;
		}
	}

	auto getWindowForEnum(Windows windowType)
	{
		switch (windowType)
		{
		case Windows::kaiser: 
			return juce::dsp::WindowingFunction<FloatType>::kaiser;

		case Windows::hann:
		case Windows::hamming:
		default:
			return juce::dsp::WindowingFunction<FloatType>::hann;
		}
	}

	void initialiseWindow(JuceWindowTypes window)
	{
		juce::dsp::WindowingFunction<FloatType>::fillWindowingTables(windowBuffer.data(), windowSize,
			juce::dsp::WindowingFunction<FloatType>::hann, false);
	}

private:	
	std::unique_ptr<juce::dsp::FFT> fft;

	// Buffers
	BlockCircularBuffer<FloatType> analysisBuffer;
	BlockCircularBuffer<FloatType> synthesisBuffer;
	std::vector<FloatType> spectralBuffer;
	std::vector<FloatType> resampleBuffer;

	// Misc state
	long incomingSampleCount = 0;
	int spectralBufferSize = 0;
	int samplesTilNextProcess = 0;
	bool isProcessing = false;

protected:
	juce::SpinLock paramLock;
	std::vector<FloatType> windowBuffer;
	float rescalingFactor = 1.f;
	int analysisHopSize = 0;
	int synthesisHopSize = 0;
	int windowSize = 0;
	int resampleSize = 0;
};

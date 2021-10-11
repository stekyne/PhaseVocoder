#pragma once

// Resample a signal to a new size using linear interpolation
// The 'originalSize' is the max size of the original signal
// The 'newSignalSize' is the size to resample to. The 'newSignal' must be at least as big as this size.
static void linearResample(const float* const originalSignal, const int originalSize,
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
		const auto wholeIndex = (int)floor(index);
		const auto fractionIndex = index - wholeIndex;
		const auto sampleA = originalSignal[wholeIndex];
		const auto sampleB = originalSignal[wholeIndex + 1];
		newSignal[i] = lerp(sampleA, sampleB, fractionIndex);
		index += scale;
	}
}

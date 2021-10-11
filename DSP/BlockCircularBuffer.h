#pragma once

#include <juce_core/juce_core.h>
#include <memory>
#include <cassert>

//#define DEBUGLOG

template<typename ElementType = float>
struct BlockCircularBuffer final
{
    BlockCircularBuffer () = default;

    BlockCircularBuffer (long newSize)
    {
        setSize (newSize, true);
    }

	void setReadHopSize (int hopSize)
	{
		readHopSize = hopSize;
	}

	auto getReadHopSize () const 
	{
		return readHopSize;
	}

	void setWriteHopSize (int hopSize)
	{
		writeHopSize = hopSize;
	}

	auto getWriteHopSize () const
	{
		return writeHopSize;
	}

	auto getReadIndex () const 
	{
		return readIndex;
	}

	auto getWriteIndex () const 
	{
		return writeIndex;
	}

    void setSize (long newSize, bool shouldClear = false)
    {
        if (newSize == length)
        {
            if (shouldClear)
                reset ();

            return;
        }

        block.allocate (newSize, shouldClear);
        length = newSize;
        writeIndex = readIndex = 0;
    }

	void setEnableLogging (const char * const bufferName, bool enabled)
	{
		name = bufferName;
	}

    void reset ()
    {
        block.clear (length);
        writeIndex = readIndex = 0;
    }

    // Read samples from the internal buffer into the 'destBuffer'
    // perform a wrap of the read if near the internal buffer boundaries
    void read (ElementType* const destBuffer, const long destLength)
    {
        const auto firstReadAmount = readIndex + destLength >= length ?
			length - readIndex : destLength;

		assert (destLength <= length);
		assert (firstReadAmount <= destLength);

		const auto internalBuffer = block.getData ();
		assert (internalBuffer != destBuffer);

		memcpy (destBuffer, internalBuffer + readIndex, sizeof (ElementType) * firstReadAmount);

		if (firstReadAmount < destLength)
		{
			memcpy (destBuffer + firstReadAmount, internalBuffer, sizeof (ElementType) * 
				(static_cast<unsigned long long>(destLength) - firstReadAmount));
		}

		readIndex += readHopSize != 0 ? readHopSize : destLength;
		readIndex %= length;

#ifdef DEBUGLOG
		printState ();
#endif
    }

    // Write all samples from the 'sourceBuffer' into the internal buffer
    // Perform any wrapping required
    void write (const ElementType* sourceBuffer, const long sourceLength)
    {
		const auto firstWriteAmount = writeIndex + sourceLength >= length ?
			length - writeIndex : sourceLength;

		auto internalBuffer = block.getData ();
		assert (internalBuffer != sourceBuffer);
		memcpy (internalBuffer + writeIndex, sourceBuffer, sizeof (ElementType) * firstWriteAmount);

		if (firstWriteAmount < sourceLength)
		{
			memcpy (internalBuffer, sourceBuffer + firstWriteAmount, sizeof (ElementType) * 
				(static_cast<unsigned long long>(sourceLength) - firstWriteAmount));
		}

		writeIndex += writeHopSize != 0 ? writeHopSize : sourceLength;
		writeIndex %= length;

		latestDataIndex = writeIndex + sourceLength % length;

#ifdef DEBUGLOG
		printState ();
#endif
    }

    // The first 'overlapAmount' of 'sourceBuffer' samples are added to the existing buffer
    // The remainder of samples are set in the buffer (overwrite)
    void overlapWrite (ElementType* const sourceBuffer, const long sourceLength)
    {
		// Since we're using a circular buffer, we have to be careful when to add samples to the existing
		// data and when to overwrite out of date samples. This number can change when modulating between
		// the pitch (which alters the size of the overlaps). The calculation below will determine the
		// index we need to "add" to and at which point we need to "set" the samples to overwrite the history
		const int writeIndexDifference = getDifferenceBetweenIndexes(writeIndex, latestDataIndex, length);
		const int overlapSampleCount = sourceLength - writeHopSize;
		const auto overlapAmount = std::min(writeIndexDifference, overlapSampleCount);
		
		auto tempWriteIndex = writeIndex;
		auto firstWriteAmount = writeIndex + overlapAmount > length ?
			length - writeIndex : overlapAmount;

		auto internalBuffer = block.getData();

		juce::FloatVectorOperations::add (internalBuffer + writeIndex, sourceBuffer, firstWriteAmount);

		if (firstWriteAmount < overlapAmount)
		{
			juce::FloatVectorOperations::add (internalBuffer, sourceBuffer + firstWriteAmount,
				overlapAmount - firstWriteAmount);
		}

		tempWriteIndex += overlapAmount;
		tempWriteIndex %= length;

		const auto remainingElements = sourceLength - overlapAmount;
		firstWriteAmount = tempWriteIndex + remainingElements > length ?
			length - tempWriteIndex : remainingElements;

		memcpy (internalBuffer + tempWriteIndex, sourceBuffer + overlapAmount, sizeof (ElementType) *
			firstWriteAmount);

		if (firstWriteAmount < remainingElements)
		{
			memcpy (internalBuffer, sourceBuffer + overlapAmount + firstWriteAmount, sizeof (ElementType) *
				(remainingElements - static_cast<unsigned long long>(firstWriteAmount)));
		}

		latestDataIndex = (writeIndex + sourceLength) % length;
		writeIndex += writeHopSize;
		writeIndex %= length;
		
#ifdef DEBUGLOG
		printState ();
#endif
    }

#ifdef DEBUGLOG
	void printState ()
	{
		DBG ("Name: " << name << juce::String::formatted (", Read Indx: %d, Write Indx: %d, Length: %d",
			readIndex, writeIndex, length));
	}
#endif

private:
	int getDifferenceBetweenIndexes(int index1, int index2, int bufferLength) 
	{
		return (index1 <= index2) ? index2 - index1 : bufferLength - index1 + index2;
	}

private:
    juce::HeapBlock<ElementType> block;
    long writeIndex = 0;
    long readIndex = 0;
    long length = 0;
	long latestDataIndex = 0;
    int writeHopSize = 0;
    int readHopSize = 0;

#ifdef DEBUG
	const char* name = "";
#endif
};

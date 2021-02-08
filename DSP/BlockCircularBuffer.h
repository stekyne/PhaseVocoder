#pragma once

#include <juce_core/juce_core.h>
#include <memory>
#include <cassert>

template<typename DataType = float>
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
		shouldLog = enabled;
	}

    void reset ()
    {
        block.clear (length);
        writeIndex = readIndex = 0;
    }

    // Read samples from the internal buffer into the 'destBuffer'
    // perform a wrap of the read if near the internal buffer boundaries
    void read (DataType* destBuffer, const long destLength)
    {
        const int firstReadAmount = readIndex + destLength >= length ?
			length - readIndex : destLength;

		assert (destLength <= length);
		assert (firstReadAmount <= destLength);

		auto internalBuffer = block.getData ();
		assert (internalBuffer != destBuffer);

		memcpy (destBuffer, internalBuffer + readIndex, sizeof (DataType) * firstReadAmount);

		if (firstReadAmount < destLength)
		{
			memcpy (destBuffer + firstReadAmount, internalBuffer, sizeof (DataType) * 
				(destLength - firstReadAmount));
		}

		readIndex += readHopSize != 0 ? readHopSize : destLength;
		readIndex %= length;

		if (shouldLog) printState ();
    }

    // Write all samples from the 'sourceBuffer' into the internal buffer
    // Perform any wrapping required
    void write (const DataType* sourceBuffer, const long sourceLength)
    {
		sampleCount += sourceLength;

		const int firstWriteAmount = writeIndex + sourceLength >= length ?
			length - writeIndex : sourceLength;

		auto internalBuffer = block.getData ();
		assert (internalBuffer != sourceBuffer);
		memcpy (internalBuffer + writeIndex, sourceBuffer, sizeof (DataType) * firstWriteAmount);

		if (firstWriteAmount < sourceLength)
		{
			memcpy (internalBuffer, sourceBuffer + firstWriteAmount, sizeof (DataType) * 
				(sourceLength - firstWriteAmount));
		}

		writeIndex += writeHopSize != 0 ? writeHopSize : sourceLength;
		writeIndex %= length;

		if (shouldLog) printState ();
    }

    // The first 'overlapAmount' of 'sourceBuffer' samples are added to the existing buffer
    // The remainder of samples are set in the buffer (overwrite)
    void overlapWrite (DataType* sourceBuffer, const long sourceLength)
    {
		const auto overlapAmount = sourceLength - writeHopSize;
        auto internalBuffer = block.getData ();
		auto tempWriteIndex = writeIndex;
		int firstWriteAmount = writeIndex + overlapAmount > length ?
			length - writeIndex : overlapAmount;

		juce::FloatVectorOperations::add (internalBuffer + writeIndex, sourceBuffer, firstWriteAmount);

		if (firstWriteAmount < overlapAmount)
		{
			juce::FloatVectorOperations::add (internalBuffer, sourceBuffer + firstWriteAmount,
				overlapAmount - firstWriteAmount);
		}

		tempWriteIndex += overlapAmount;
		tempWriteIndex %= length;

		const int remainingElements = sourceLength - overlapAmount;
		firstWriteAmount = tempWriteIndex + remainingElements > length ?
			length - tempWriteIndex : remainingElements;

		memcpy (internalBuffer + tempWriteIndex, sourceBuffer + overlapAmount, sizeof (DataType) *
			firstWriteAmount);

		if (firstWriteAmount < remainingElements)
		{
			memcpy (internalBuffer, sourceBuffer + overlapAmount + firstWriteAmount, sizeof (DataType) *
				(remainingElements - firstWriteAmount));
		}

		writeIndex += writeHopSize;
		writeIndex %= length;

		if (shouldLog) printState ();
    }

	void printState ()
	{
#ifdef DEBUG
		DBG ("Name: " << name << juce::String::formatted (", Read Indx: %d, Write Indx: %d, Length: %d",
			readIndex, writeIndex, length));
#endif
	}

private:
    juce::HeapBlock<DataType> block;
    long writeIndex = 0;
    long readIndex = 0;
    long length = 0;
	long sampleCount = 0;
    int writeHopSize = 0;
    int readHopSize = 0;
	bool shouldLog = false;

#ifdef DEBUG
	const char* name = "";
#endif
};

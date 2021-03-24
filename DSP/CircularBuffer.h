#pragma once

// A circular buffer that operates on blocks of data rather than
// single elements. Hop size can be set for read or write 
// This will increment the appropriate index by the hop amount
template <typename DataType = float>
class CircularBlockBuffer
{
public:
    using SampleType = DataType;

    CircularBlockBuffer (size_t numChans, size_t bufferSize) :
        internalBuffer ((int)numChans, (int)bufferSize)
    {
        clear ();
    }

    CircularBlockBuffer (size_t numChans, size_t bufferSize, 
                         size_t readHopSize_, size_t writeHopSize_) :
        readHopSize (readHopSize_),
        writeHopSize (writeHopSize_),
        internalBuffer ((int)numChans, (int)bufferSize)
    {
        clear ();
    }

    void setSize (size_t chanSize, size_t newSize)
    {
        internalBuffer.setSize ((int)chanSize, (int)newSize);
    }

    void setReadHopSize (size_t newSize)
    {
        readHopSize = newSize;
    }

    auto getReadHopSize () const
    {
        return readHopSize;
    }

    void setWriteHopSize (size_t newSize)
    {
        writeHopSize = newSize;
    }

    auto getWriteHopSize () const
    {
        return writeHopSize;
    }

    void clear ()
    {
        internalBuffer.clear ();
        readIndex = 0;
        writeIndex = 0;
    }

    /*void read (AudioBuffer<DataType>& readBuffer)
    {
        const auto numSamples = readBuffer.getNumSamples ();
        bool hasWrapped = false;

        for (auto i = 0; i < readBuffer.getNumChannels (); ++i)
        {
            hasWrapped = moveData (readBuffer.getWritePointer (i) + readIndex, 
                internalBuffer.getWritePointer (i),
                numSamples, readIndex);
        }

        incrementIndex (readIndex, readHopSize == 0 ? 
            numSamples : readHopSize, hasWrapped);
    }*/
    
    void read (AudioBuffer<DataType>& readBuffer)
    {

    }

    void write (AudioBuffer<DataType>& buffer)
    {
        const auto numSamples = buffer.getNumSamples ();
        bool hasWrapped = false;

        if (writeHopSize == 0)
        {
            for (auto i = 0; i < buffer.getNumChannels (); ++i)
            {
                hasWrapped = moveData (internalBuffer.getWritePointer (i),
                    buffer.getWritePointer (i), numSamples, writeIndex);
            }
        }
        else
        {
            for (auto i = 0; i < buffer.getNumChannels (); ++i)
            {
                overlapAdd (internalBuffer.getWritePointer (i),
                    buffer.getWritePointer (i), writeHopSize,
                    numSamples, writeIndex);
            }
        }

        incrementIndex (writeIndex, writeHopSize == 0 ?
            numSamples : writeHopSize, hasWrapped);
    }

private:
    // Returns true if a wrap occurred
    bool moveData (DataType* destBuffer, DataType* sourceBuffer, 
                   size_t numSrcElements, size_t destIndex)
    {
        const size_t maxSamples = internalBuffer.getNumSamples ();

        jassert (isPositiveAndBelow (destIndex, maxSamples));
        jassert (isPositiveAndBelow (numSrcElements, maxSamples));
        
        const size_t firstAmount = destIndex + numSrcElements >= maxSamples ?
            maxSamples - destIndex : numSrcElements;

        jassert (isPositiveAndNotGreaterThan (firstAmount, numSrcElements));
        memcpy (destBuffer + destIndex, sourceBuffer, 
            sizeof (DataType) * firstAmount);

        // Written everything so bail out
        if (firstAmount == numSrcElements)
            return false;

        // Perform a second memcpy if wrapping the end of the buffer 
        const auto secondAmount = numSrcElements - firstAmount;
        jassert (destIndex + firstAmount <= maxSamples);
        jassert (firstAmount + secondAmount == numSrcElements);
        memcpy (destBuffer, sourceBuffer + firstAmount, 
            sizeof (DataType) * secondAmount);

        return true;
    }

    // Add the first overlap amount of samples to the destination buffer
    // Move the remaining samples into the dest buffer (overwrite what's there)
    // The numElements is the total amount including the overlap amount
    // The overlapAmount must be less than the numElements
    // Returns true on wrapped write
    bool overlapAdd (DataType* destBuffer, DataType* sourceBuffer, size_t overlapAmount, 
                     size_t numSrcElements, size_t destIndex)
    {
        jassert (destIndex < internalBuffer.getNumSamples ());
        jassert (overlapAmount < numSrcElements);
        jassert (numSrcElements < internalBuffer.getNumSamples ());

        // Calculate write amount from buffer end
        auto remainingSamples = numSrcElements;
        auto hasWrapped = false;
        
        const size_t numSamples = internalBuffer.getNumSamples ();
        const size_t firstWriteAmount = destIndex + numSrcElements >= numSamples ?
            numSamples - destIndex :
            destIndex + numSrcElements;
        
        // Test if a wrap will occur for overlap add, moveData will handle remaining wraps
        if (overlapAmount < firstWriteAmount)
        {
            FloatVectorOperations::add (destBuffer + destIndex, sourceBuffer, 
                (int)overlapAmount);
        }
        else
        {
            const size_t secondWriteAmount = overlapAmount - firstWriteAmount;
            FloatVectorOperations::add (destBuffer + destIndex, sourceBuffer, (int)overlapAmount);
            FloatVectorOperations::add (destBuffer, sourceBuffer, (int)secondWriteAmount);
            hasWrapped = true;
        }

        remainingSamples -= overlapAmount;
        destIndex += overlapAmount;

        const auto hasWrapped2 = 
            moveData (destBuffer, sourceBuffer, remainingSamples, destIndex);
        
        return (hasWrapped || hasWrapped2);
    }

    inline void incrementIndex (size_t& index, size_t incrementAmount, bool hasWrapped)
    {
        jassert (index < internalBuffer.getNumSamples ());

        if (hasWrapped)
            index = (index - internalBuffer.getNumSamples ()) + incrementAmount;
        else 
        {
            index += incrementAmount;
            index %= (size_t)internalBuffer.getNumSamples ();
        }
    }

private:
    AudioBuffer<DataType> internalBuffer;
    HeapBlock<DataType> inBuffer;
    size_t readIndex = 0;
    size_t writeIndex = 0;
    size_t readHopSize = 0;
    size_t writeHopSize = 0;
};

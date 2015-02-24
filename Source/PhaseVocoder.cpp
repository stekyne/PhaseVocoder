#include "PhaseVocoder.h"

PhaseVocoder::PhaseVocoder ()
    :   pv_left (new PV (1024, 1024, 256)),
        pv_right (new PV (1024, 1024, 256))
{
}

PhaseVocoder::~PhaseVocoder ()
{
}

void PhaseVocoder::prepareToPlay (int /*samplesPerBlockExpected*/, double /*sampleRate*/)
{
}

void PhaseVocoder::releaseResources ()
{
}

void PhaseVocoder::getNextAudioBlock (const AudioSourceChannelInfo &bufferToFill)
{
    auto* dataLeft = bufferToFill.buffer->getWritePointer (0, 0);
    auto* dataRight = bufferToFill.buffer->getWritePointer (1, 0);

    pv_left->processOffline (dataLeft, dataLeft, 256, bufferToFill.numSamples);
    pv_right->processOffline (dataRight, dataRight, 256, bufferToFill.numSamples);
}

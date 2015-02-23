#include "AudioPositionalSource.h"

AudioPositionalSource::AudioPositionalSource () 
{}

AudioPositionalSource::~AudioPositionalSource () 
{}

void AudioPositionalSource::setNextReadPosition (int newPosition) 
{}

int64 AudioPositionalSource::getNextReadPosition () const 
{
    return 0;
}

int64 AudioPositionalSource::getTotalLength () const 
{
    return 0;
}

bool AudioPositionalSource::isLooping () const 
{
    return false;
}

void AudioPositionalSource::setLooping (bool shouldLoop) 
{}

void AudioPositionalSource::prepareToPlay (int samplesPerBlockExpected, double sampleRate) 
{}

void AudioPositionalSource::releaseResources ()
{}

void AudioPositionalSource::getNextAudioBlock (const AudioSourceChannelInfo &bufferToFill)
{}

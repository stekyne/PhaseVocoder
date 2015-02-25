#include "AudioFileSource.h"

AudioFileSource::AudioFileSource ()
{
	setSource (0);
}

AudioFileSource::AudioFileSource (AudioFormatReader* const reader)
    :   audioReaderSource (new AudioFormatReaderSource (reader, true))
{
    setSource (audioReaderSource);
}

AudioFileSource::~AudioFileSource ()
{
    stop ();
	removeAllChangeListeners ();
}

void AudioFileSource::setFile (AudioFormatReader* reader)
{
	stop ();
	auto newAudioReaderSource = new AudioFormatReaderSource (reader, true);
    setSource (newAudioReaderSource);
    audioReaderSource = newAudioReaderSource;
}
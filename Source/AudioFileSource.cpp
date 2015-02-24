#include "AudioFileSource.h"

AudioFileSource::AudioFileSource ()
{
	setSource (0);
}

AudioFileSource::AudioFileSource (AudioFormatReader* const reader)
	:	audio_reader_source	(new AudioFormatReaderSource (reader, true))
{
	setSource (audio_reader_source);
}

AudioFileSource::~AudioFileSource ()
{
    stop ();
	removeAllChangeListeners ();
}

void AudioFileSource::setFile (AudioFormatReader* reader)
{
	stop ();
	auto newaudio_reader_source = new AudioFormatReaderSource (reader, true);
    setSource (newaudio_reader_source);
    audio_reader_source = newaudio_reader_source;
}
/*
  ==============================================================================

    AudioFileSource.cpp
    Created: 24 Mar 2011 11:11:50am
    Author:  Administrator

  ==============================================================================
*/

#include "AudioFileSource.h"

AudioFileSource::AudioFileSource()
{
	setSource( 0 );
}

AudioFileSource::AudioFileSource( AudioFormatReader* const reader )
	:	audio_reader_source	( new AudioFormatReaderSource( reader, true ) )
{
	setSource( audio_reader_source );
}

AudioFileSource::~AudioFileSource()
{
	setSource( 0 );
	removeAllChangeListeners();
}

void AudioFileSource::setFile( AudioFormatReader* reader )
{
	stop();
	setSource( 0 );
	audio_reader_source = new AudioFormatReaderSource( reader, true );
	setSource( audio_reader_source );
}
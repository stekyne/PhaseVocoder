/*
  ==============================================================================

    AudioFileSource.h
    Created: 24 Mar 2011 11:11:50am
    Author:  Administrator

  ==============================================================================
*/

#ifndef __AUDIOFILESOURCE_H_9DE69AC__
#define __AUDIOFILESOURCE_H_9DE69AC__

#include "../JuceLibraryCode/JuceHeader.h"


class AudioFileSource	:	public AudioTransportSource
{
public:
	AudioFileSource();
	AudioFileSource( AudioFormatReader* const reader );
	~AudioFileSource();

	void setFile( AudioFormatReader* reader );

private:
	ScopedPointer <AudioFormatReaderSource> audio_reader_source;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR( AudioFileSource );
};


#endif  // __AUDIOFILESOURCE_H_9DE69AC__

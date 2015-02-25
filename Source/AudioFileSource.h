#ifndef __AUDIOFILESOURCE_H_9DE69AC__
#define __AUDIOFILESOURCE_H_9DE69AC__

#include "../JuceLibraryCode/JuceHeader.h"

class AudioFileSource	:	public AudioTransportSource
{
public:
	AudioFileSource ();
	AudioFileSource (AudioFormatReader* const reader);
	~AudioFileSource ();

	void setFile (AudioFormatReader* reader);

    void setLooping (bool shouldLoop)
    {
        audioReaderSource->setLooping (shouldLoop);
    }

private:
    ScopedPointer<AudioFormatReaderSource> audioReaderSource;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioFileSource);
};

#endif

/*
  ==============================================================================

    AudioPositionalSource.h
    Created: 24 Mar 2011 6:19:39pm
    Author:  Administrator

  ==============================================================================
*/

#ifndef __AUDIOPOSITIONALSOURCE_H_367BA304__
#define __AUDIOPOSITIONALSOURCE_H_367BA304__

#include "../JuceLibraryCode/JuceHeader.h"

class AudioPositionalSource	:	public PositionableAudioSource
{
public:
	AudioPositionalSource();
	~AudioPositionalSource();

	void setNextReadPosition (int newPosition);
	int64 getNextReadPosition () const;
	int64 getTotalLength () const;
	bool isLooping () const;
	void setLooping (bool shouldLoop);

	void prepareToPlay( int samplesPerBlockExpected, double sampleRate );
	void releaseResources();
	void getNextAudioBlock( const AudioSourceChannelInfo &bufferToFill );

private:

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR( AudioPositionalSource );
};

#endif  // __AUDIOPOSITIONALSOURCE_H_367BA304__

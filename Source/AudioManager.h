#ifndef __AUDIOMANAGER_23ADECB__
#define __AUDIOMANAGER_23ADECB__

#include "../JuceLibraryCode/JuceHeader.h"
#include "AudioFileSource.h"
#include "PhaseVocoder.h"

class AudioManager	:	public ChangeListener,
						public AudioIODeviceCallback
{
public:
	AudioManager ();
	~AudioManager ();

	void audioDeviceIOCallback (const float **inputChannelData, int totalNumInputChannels, 
								float **outputChannelData, int totalNumOutputChannels, 
								int numSamples);
 	void audioDeviceAboutToStart (AudioIODevice *device);
 	void audioDeviceStopped ();

	void changeListenerCallback (ChangeBroadcaster *source);
	
	void setGain (float gain)
	{
        audioSource->setGain (gain);
	}

	void play ()
	{
        audioSource->start ();
        read_pos = audioSource->getNextReadPosition ();
	}

	void stop ()
	{
        audioSource->stop ();
	}

	void setLooping (bool shouldLoop)
	{
        audioSource->setLooping (shouldLoop);
	}

	void setPosition (double pos)
	{
		audioSource->setPosition (pos);
        read_pos = audioSource->getNextReadPosition ();
	}

	void setPitch (float newValue)
	{
        phaseVocoder->setPitch (newValue);
	}

	void setTimeScale (float newValue)
	{
        phaseVocoder->setTimeScale (newValue);
	}

	void setPhaseLock (bool state)
	{
        phaseVocoder->setPhaseLock (state);
	}

	double getPlaybackPosition () const
	{
        return audioSource->getCurrentPosition ();
	}

	double getPlaybackLength () const
	{
        return audioSource->getLengthInSeconds ();
	}

	void setFileSource (AudioFormatReader* const reader)
	{
        audioSource->setFile (reader);
	}

    ScopedPointer<AudioDeviceSelectorComponent> getSelector (
        int width = 500, int height = 400);

    const double getCPU () const { return deviceManager->getCpuUsage (); }

	juce_DeclareSingleton (AudioManager, true);

private:
	ScopedPointer<AudioDeviceManager> deviceManager;
	ScopedPointer<AudioFileSource> audioSource;
	ScopedPointer<AudioSampleBuffer> audioBuffer;
	ScopedPointer<PhaseVocoder> phaseVocoder;
	ScopedPointer<MixerAudioSource> mixerSource;

    int64 read_pos {0};
    int64 buf_cnt {0};
    bool shouldLoop {false};
};

#endif
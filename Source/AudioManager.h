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
	
	void setGain (double gain)
	{
		audio_source->setGain (gain);
	}

	void play ()
	{
		audio_source->start ();
		read_pos = audio_source->getNextReadPosition ();
	}

	void stop ()
	{
		audio_source->stop();
	}

	void loop (bool shouldLoop)
	{
        this->shouldLoop = shouldLoop;
	}

	void setPosition (double pos)
	{
		audio_source->setPosition (pos);
		read_pos = audio_source->getNextReadPosition ();
	}

	void setPitch (double newValue)
	{
		phase_vocoder->setPitch (newValue);
	}

	void setTimeScale (double newValue)
	{
		phase_vocoder->setTimeScale (newValue);
	}

	void setPhaseLock (bool state)
	{
		phase_vocoder->setPhaseLock (state);
	}

	double getPlaybackPosition () const
	{
		return audio_source->getCurrentPosition ();	
	}

	double getPlaybackLength () const
	{
		return audio_source->getLengthInSeconds ();
	}

	void setFileSource (AudioFormatReader* const reader)
	{
		audio_source->setFile (reader);
	}

    ScopedPointer<AudioDeviceSelectorComponent> getSelector (
        int width = 500, int height = 400);

	const double getCPU () const { return device_manager->getCpuUsage (); }

	juce_DeclareSingleton (AudioManager, true);

private:
	ScopedPointer<AudioDeviceManager> device_manager;
	ScopedPointer<AudioFileSource> audio_source;
	ScopedPointer<AudioSampleBuffer> audio_buf;
	ScopedPointer<PhaseVocoder> phase_vocoder;
	ScopedPointer<MixerAudioSource> mixer_source;

    int64 read_pos {0};
    int64 buf_cnt {0};
    bool shouldLoop {false};
};

#endif
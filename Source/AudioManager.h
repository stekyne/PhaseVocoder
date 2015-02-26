#ifndef __AUDIOMANAGER_23ADECB__
#define __AUDIOMANAGER_23ADECB__

#include "../JuceLibraryCode/JuceHeader.h"
#include "AudioFileSource.h"
#include "PhaseVocoder.h"

class AudioManager	:	public ChangeListener,
                        public ChangeBroadcaster,
						public AudioIODeviceCallback
{
public:
	AudioManager (String filename);
	~AudioManager ();

	void audioDeviceIOCallback (const float **inputChannelData, int totalNumInputChannels, 
								float **outputChannelData, int totalNumOutputChannels, 
								int numSamples);
 	void audioDeviceAboutToStart (AudioIODevice *device);
 	void audioDeviceStopped ();

    void changeListenerCallback (ChangeBroadcaster *source) {}
	
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

	bool setFileSource (const File& audioFile)
	{
        currentAudioFile = audioFile;
        AudioFormatReader* reader = formatManager->createReaderFor (audioFile);

        if (reader != nullptr)
        {
            audioSource->setFile (reader);
            sendSynchronousChangeMessage ();
            return true;
        }

        return false;
	}

    File getCurrentAudioFile () const
    {
        return currentAudioFile;
    }

    ScopedPointer<AudioDeviceSelectorComponent> getSelector (
        int width = 500, int height = 400);

    double getCPU () const 
    { 
        return deviceManager->getCpuUsage (); 
    }

    AudioFormatManager* getFormatManager () const
    {
        return formatManager;
    }

private:
	ScopedPointer<AudioDeviceManager> deviceManager;
	ScopedPointer<AudioFileSource> audioSource;
    ScopedPointer<AudioFormatManager> formatManager;
	ScopedPointer<AudioSampleBuffer> audioBuffer;
	ScopedPointer<PhaseVocoder> phaseVocoder;
	ScopedPointer<MixerAudioSource> mixerSource;
    File currentAudioFile;
    int64 read_pos {0};
    int64 buf_cnt {0};
    bool shouldLoop {false};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioManager);
};

#endif
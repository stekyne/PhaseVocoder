#include "AudioManager.h"

#define MAX_BUF_SIZE ((1024*2)+256)

AudioManager::AudioManager (String filename) :
	audioBuffer (2, MAX_BUF_SIZE)
{
	const String result (deviceManager.initialise (0, 2, 0, true));

	// 'result' will be empty if 'initialise' succeeded in opening a device
    if (result.isNotEmpty ())
	{
		DBG ("Error: A device could not be opened for playback.");
		AlertWindow::showMessageBoxAsync (  
			AlertWindow::WarningIcon,
			"Audio Device Error",
			"Could not open a suitable device or no device present");
	}
	else
	{
		audioSource.addChangeListener (this);
		deviceManager.addAudioCallback (this);
		deviceManager.addChangeListener (this);
	}

    formatManager.registerBasicFormats ();

    // User supplied audio file in command line
    if (!filename.isEmpty ())
    {
        setFileSource (filename);
    }
}

AudioManager::~AudioManager ()
{
	deviceManager.removeAllChangeListeners ();
	audioSource.setSource (0);
	deviceManager.removeAudioCallback (this);
}

void AudioManager::audioDeviceIOCallback( 
    const float** /*inputChannelData*/, int /*totalNumInputChannels*/, 
    float **outputChannelData, int totalNumOutputChannels, 
    int numSamples)
{
	if (audioSource.isPlaying())
	{
		// The buffer size must be less than 1024
		jassert (numSamples <= 1024);
        jassert (totalNumOutputChannels >= 2);
		
		float* outputLeft = outputChannelData[0];
		float* outputRight = outputChannelData[1];

		audioBuffer.clear();

		AudioSourceChannelInfo output_buf;
		output_buf.buffer = &audioBuffer;
		output_buf.numSamples = MAX_BUF_SIZE;
		output_buf.startSample = 0;

		audioSource.getNextAudioBlock (output_buf);
		
        bufferCount += numSamples;

		if (bufferCount >= 256)
		{
			int64_t div = floor (bufferCount / 256.f);
            // FIXME suspect code, needs investigating
			readPosition += (int64_t)(256 * div * phaseVocoder.getTimeScale ());
			bufferCount %= 256;
		}

		audioSource.setNextReadPosition (readPosition);

		output_buf.numSamples = numSamples;
		//phase_vocoder->getNextAudioBlock (output_buf);

		auto dataLeft = audioBuffer.getReadPointer (0, 0);
		auto dataRight = audioBuffer.getReadPointer (1, 0);

		for (int s = 0; s < numSamples; ++s)
		{
			outputLeft[s]  = dataLeft[s];
			outputRight[s] = dataRight[s];
		}
	}
	else 
	{
		zeromem (outputChannelData[0], sizeof(float) * numSamples);
		zeromem (outputChannelData[1], sizeof(float) * numSamples);
	}
}

void AudioManager::audioDeviceAboutToStart (AudioIODevice* device)
{
	audioSource.prepareToPlay (device->getCurrentBufferSizeSamples(),
								device->getCurrentSampleRate());
}

void AudioManager::audioDeviceStopped ()
{
	audioSource.releaseResources ();
}

ScopedPointer<AudioDeviceSelectorComponent> AudioManager::getSelector (
    int width, int height)
{
	ScopedPointer<AudioDeviceSelectorComponent> deviceSelector 
		 (new AudioDeviceSelectorComponent (deviceManager,
											0, 2, 0, 2, 
											true, true, true, false));

	deviceSelector->setSize (width, height);
	return deviceSelector;
}

#include "AudioManager.h"

#define MAX_BUF_SIZE ((1024*2)+256)

AudioManager::AudioManager ()
	:	device_manager (new AudioDeviceManager),
		mixer_source (new MixerAudioSource),
		audio_source (new AudioFileSource),
		phase_vocoder (new PhaseVocoder),
		audio_buf (new AudioSampleBuffer (2, MAX_BUF_SIZE))
{
	const String result (device_manager->initialise (0, 2, 0, true));

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
		audio_source->addChangeListener (this);
		device_manager->addAudioCallback (this);
		device_manager->addChangeListener (this);
	}
}

AudioManager::~AudioManager ()
{
	device_manager->removeAllChangeListeners ();
	audio_source->setSource (0);
	device_manager->removeAudioCallback (this);
	clearSingletonInstance ();
}

void AudioManager::audioDeviceIOCallback( 
    const float** /*inputChannelData*/, int totalNumInputChannels, 
    float **outputChannelData, int totalNumOutputChannels, 
    int numSamples)
{
	if (audio_source->isPlaying())
	{
		// The buffer size must be less than 1024
		jassert (numSamples <= 1024);
		
		float* outputLeft = outputChannelData[0];
		float* outputRight = outputChannelData[1];

		audio_buf->clear();

		AudioSourceChannelInfo output_buf;
		output_buf.buffer = audio_buf;
		output_buf.numSamples = MAX_BUF_SIZE;
		output_buf.startSample = 0;

		audio_source->getNextAudioBlock (output_buf);
		
        buf_cnt += numSamples;
		if (buf_cnt >= 256)
		{
			int64 div = floor (buf_cnt / 256);
            // FIXME suspect code, needs investigating
			read_pos += 256 * div * phase_vocoder->getTimeScale ();
			buf_cnt %= 256;
		}

		audio_source->setNextReadPosition (read_pos);

		output_buf.numSamples = numSamples;
		phase_vocoder->getNextAudioBlock (output_buf);

		auto dataLeft = audio_buf->getReadPointer (0, 0);
		auto dataRight = audio_buf->getReadPointer (1, 0);

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

	for (int chan = totalNumInputChannels; chan < totalNumOutputChannels; ++chan)
	{
		zeromem (outputChannelData[chan], sizeof(float) * numSamples);
	}
}

void AudioManager::audioDeviceAboutToStart (AudioIODevice* device)
{
	audio_source->prepareToPlay (device->getCurrentBufferSizeSamples(),
								 device->getCurrentSampleRate());
}

void AudioManager::audioDeviceStopped ()
{
	audio_source->releaseResources ();
}

void AudioManager::changeListenerCallback (ChangeBroadcaster* /*source*/)
{
	if (audio_source->hasStreamFinished ())
	{
        setPosition (0.0);

		if (shouldLoop)
			audio_source->start ();
	}
}

ScopedPointer<AudioDeviceSelectorComponent> AudioManager::getSelector (
    int width, int height)
{
	ScopedPointer<AudioDeviceSelectorComponent> deviceSelector 
		 (new AudioDeviceSelectorComponent (*device_manager,
											0, 2, 0, 2, 
											true, true, true, false));

	deviceSelector->setSize (width, height);
	return deviceSelector;
}

juce_ImplementSingleton (AudioManager);

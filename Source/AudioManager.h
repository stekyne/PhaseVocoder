/**
	Class to manage the audio devices and audio callback

	Essentially the focal point of all the audio processing
*/

#ifndef __AUDIOMANAGER_23ADECB__
#define __AUDIOMANAGER_23ADECB__

#include "../JuceLibraryCode/JuceHeader.h"
#include "AudioFileSource.h"
#include "PhaseVocoder.h"

class AudioManager	:	public ChangeListener,
						//public AudioSourcePlayer
						public AudioIODeviceCallback
{
public:
	AudioManager();
	~AudioManager();

	//==================================================================
	void audioDeviceIOCallback( const float **inputChannelData, int totalNumInputChannels, 
								float **outputChannelData, int totalNumOutputChannels, 
								int numSamples );
 	void audioDeviceAboutToStart( AudioIODevice *device );
 	void audioDeviceStopped();

	//==================================================================
	/** The audioDeviceManager will broadcast changes which will be
		picked up by this callback */
	void changeListenerCallback( ChangeBroadcaster *source );
	

	void setGain( const double gain )
	{
		audio_source->setGain( gain );
	}

	void play()
	{
		audio_source->start();
		read_pos = audio_source->getNextReadPosition();
	}

	void stop()
	{
		audio_source->stop();
	}

	void loop( const bool state )
	{
		if( state )
		{
			shouldLoop = true;
		}
		else
		{
			shouldLoop = false;
		}
	}

	void setPosition( const double pos )
	{
		audio_source->setPosition( pos );
		read_pos = audio_source->getNextReadPosition();
	}

	void setPitch( const double newValue )
	{
		phase_vocoder->setPitch( newValue );
	}

	void setTimeScale( const double newValue )
	{
		phase_vocoder->setTimeScale( newValue );
	}

	void setPhaseLock( const bool state )
	{
		phase_vocoder->setPhaseLock( state );
	}

	double getPlaybackPosition() const
	{
		return audio_source->getCurrentPosition();	
	}

	double getPlaybackLength() const
	{
		return audio_source->getLengthInSeconds();
	}

	void setFileSource( AudioFormatReader* const reader )
	{
		audio_source->setFile( reader );
	}

	/** Return a AudioDevice Settings component to let the user select the 
		correct sound card and midi options */
	ScopedPointer<AudioDeviceSelectorComponent> getSelector( 
		const int width = 500, const int height = 450 );

	/** Get CPU usage for the callback */
	const double getCPU() const { return device_manager->getCpuUsage(); }

	juce_DeclareSingleton( AudioManager, true );

private:
	/** Audio Device manager sets up the soundcard for playback */
	ScopedPointer <AudioDeviceManager> device_manager;
	
	/** Source of audio for this application */
	ScopedPointer <AudioFileSource> audio_source;
	ScopedPointer <AudioSampleBuffer> audio_buf;

	ScopedPointer <PhaseVocoder> phase_vocoder;

	/** Main transport source, controls when to play audio from the sequencer */
	ScopedPointer <MixerAudioSource> mixer_source;

	/** Read position for the audio file */
	int64 read_pos;
	int64 buf_cnt;

	bool shouldLoop;

	//JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR( AudioManager );
};

#endif
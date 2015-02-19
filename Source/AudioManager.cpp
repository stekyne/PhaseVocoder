/**
	Class to manage the audio devices and audio callback

	Essentially the focal point of all the audio processing
*/

#include "AudioManager.h"

#define MAX_BUF_SIZE ((1024*2)+256)

AudioManager::AudioManager()
	:	device_manager	( new AudioDeviceManager ),
		mixer_source	( new MixerAudioSource ),
		audio_source	( new AudioFileSource ),
		phase_vocoder	( new PhaseVocoder ),
		audio_buf		( new AudioSampleBuffer( 2, MAX_BUF_SIZE ) ),
		read_pos		( 0 ),
		buf_cnt			( 0 ),
		shouldLoop		( false )
{
	/** Configure AudioDeviceManager */
	const String error( device_manager->initialise( 0, 2, 0, true ) );

	/** 'error' will be empty if 'initialise' succeeded in opening a device */
	if( error.isNotEmpty() )
	{
		DBG( L"Error: A device could not be opened for playback." );

		AlertWindow::showMessageBoxAsync(  
			AlertWindow::WarningIcon,
			L"Audio Device Error",
			L"Could not open a suitable device or no device present" );
	}
	else
	{
		/** Listen for changes from the transport source */
		audio_source->addChangeListener( this );

		/** Add an audio callback that will do the audio processing */
		device_manager->addAudioCallback( this );

		/** Listen to any changes from the audio device manager */
		device_manager->addChangeListener( this );
	}
}

AudioManager::~AudioManager()
{
	device_manager->removeAllChangeListeners();
	audio_source->setSource( 0 );
	device_manager->removeAudioCallback( this );

	clearSingletonInstance();
}

void AudioManager::audioDeviceIOCallback( 
								const float **inputChannelData,  int totalNumInputChannels, 
								float		**outputChannelData, int totalNumOutputChannels, 
								int			numSamples )
{
	if( audio_source->isPlaying() )
	{
		/** The buffer size must be less than 1024 */
		jassert( numSamples <= 1024 );
		
		float* outputLeft	= outputChannelData[0];
		float* outputRight	= outputChannelData[1];

		/** Data buffer */
		audio_buf->clear();

		/** Output info */
		AudioSourceChannelInfo output_buf;
		output_buf.buffer		= audio_buf;
		output_buf.numSamples   = MAX_BUF_SIZE;
		output_buf.startSample	= 0;

		/** Read the next block from the file */
		audio_source->getNextAudioBlock( output_buf );
	
		/** Determine the next read position for the audio file */
		buf_cnt += numSamples;

		if( buf_cnt >= 256 )
		{
			int64 div = floorl( buf_cnt / 256 );
			read_pos += 256 * div * phase_vocoder->getTimeScale();
			buf_cnt  %= 256;
		}

		audio_source->setNextReadPosition( read_pos );

		/** Apply phase vocoder to this data */
		output_buf.numSamples = numSamples;
		phase_vocoder->getNextAudioBlock( output_buf );

		/** Put this data in the output channels */
		const float *dataLeft  = audio_buf->getSampleData( 0, 0 );
		const float *dataRight = audio_buf->getSampleData( 1, 0 );

		for( int s=0; s < numSamples; ++s )
		{
			outputLeft[s]  = dataLeft[s];
			outputRight[s] = dataRight[s];
		}
	}
	else 
	{
		zeromem( outputChannelData[0], sizeof(float) * numSamples );
		zeromem( outputChannelData[1], sizeof(float) * numSamples );
	}

	/** Clear remaining channels */
	for( int chan = 2; chan < totalNumOutputChannels; ++chan )
	{
		zeromem( outputChannelData[chan], sizeof(float) * numSamples );
	}
}

void AudioManager::audioDeviceAboutToStart( AudioIODevice *device )
{
	audio_source->prepareToPlay( device->getCurrentBufferSizeSamples(),
								 device->getCurrentSampleRate() );
}

void AudioManager::audioDeviceStopped()
{
	audio_source->releaseResources();
}

void AudioManager::changeListenerCallback( ChangeBroadcaster *source )
{
	/** Audio Transport has started / stopped */
	if( audio_source->hasStreamFinished() )
	{
		if( shouldLoop )
		{
			setPosition( 0.0 );
			audio_source->start();
		}
		else
		{
			setPosition( 0.0 );
		}
	}
}

ScopedPointer<AudioDeviceSelectorComponent> AudioManager::getSelector( 
		const int width, const int height )
{
	ScopedPointer<AudioDeviceSelectorComponent> deviceSelector 
		= new AudioDeviceSelectorComponent ( *device_manager,
											 0, 2, 0, 2, 
											 true, true, true, false );

	deviceSelector->setSize( width, height );

	return deviceSelector;
}

juce_ImplementSingleton( AudioManager );

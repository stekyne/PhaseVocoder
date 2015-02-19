/*
  ==============================================================================

    PhaseVocoder.h
    Created: 24 Mar 2011 4:52:04pm
    Author:  Administrator

  ==============================================================================
*/

#ifndef __PHASEVOCODER_H_14521526__
#define __PHASEVOCODER_H_14521526__

#include "../JuceLibraryCode/JuceHeader.h"
#include "PV.h"

class PhaseVocoder	:	public AudioSource
{
public:
	PhaseVocoder();
	~PhaseVocoder();

	void prepareToPlay( int samplesPerBlockExpected, double sampleRate );
	void releaseResources();
	void getNextAudioBlock( const AudioSourceChannelInfo &bufferToFill );

	void reset()
	{
		//pv_left->reset();
		//pv_right->reset();
	}

	void setPitch( const double pitch_ratio )
	{
		this->pitch_ratio = pitch_ratio;
		pv_left->setPitch( pitch_ratio );
		pv_right->setPitch( pitch_ratio );
	}
	
	void setTimeScale( const double time_ratio )
	{
		this->time_ratio = time_ratio;
		pv_left->setTimeScale( time_ratio );
		pv_right->setTimeScale( time_ratio );
	}

	double getPitch() const
	{
		return pitch_ratio;
	}

	float getTimeScale() const 
	{
		return time_ratio;
	}
	
	void setPhaseLock( bool state )
	{
		isLocked = state;
		pv_left->setPhaseLock( state );
		pv_right->setPhaseLock( state );
	}

	bool getPhaseLock() const
	{
		return isLocked;
	}

private:
	ScopedPointer <PV> pv_left;
	ScopedPointer <PV> pv_right;

	double	pitch_ratio, time_ratio;
	bool	isLocked;

	//=========================================================================
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR( PhaseVocoder );
};

#endif  // __PHASEVOCODER_H_14521526__

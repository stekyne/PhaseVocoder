#ifndef __PHASEVOCODER_H_14521526__
#define __PHASEVOCODER_H_14521526__

#include "../JuceLibraryCode/JuceHeader.h"
#include "PV.h"

class PhaseVocoder  :   public AudioSource
{
public:
	PhaseVocoder ();
	~PhaseVocoder ();

	void prepareToPlay (int samplesPerBlockExpected, double sampleRate);
	void releaseResources ();
	void getNextAudioBlock (const AudioSourceChannelInfo &bufferToFill);

	void setPitch (float pitch_ratio)
	{
		this->pitch_ratio = pitch_ratio;
		pv_left->setPitch (pitch_ratio);
		pv_right->setPitch (pitch_ratio);
	}
	
	void setTimeScale (float time_ratio)
	{
		this->time_ratio = time_ratio;
		pv_left->setTimeScale (time_ratio);
		pv_right->setTimeScale (time_ratio);
	}

	double getPitch () const
	{
		return pitch_ratio;
	}

	float getTimeScale () const 
	{
		return time_ratio;
	}
	
	void setPhaseLock (bool state)
	{
		isLocked = state;
		pv_left->setPhaseLock (state);
		pv_right->setPhaseLock (state);
	}

	bool getPhaseLock () const
	{
		return isLocked;
	}

private:
	ScopedPointer<PV> pv_left;
	ScopedPointer<PV> pv_right;

    float pitch_ratio {1.f}, time_ratio {1.f};
    bool isLocked {true};

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PhaseVocoder);
};

#endif


#ifndef __JUCE_HEADER_8F8F37798A797554__
#define __JUCE_HEADER_8F8F37798A797554__

#include "../JuceLibraryCode/JuceHeader.h"
#include "WaveformDisplayComponent.h"

class PVMainComponent  : public Component,
                         public FilenameComponentListener,
                         public Timer,
                         public ButtonListener,
                         public SliderListener,
                         public ChangeListener
{
public:
    PVMainComponent (AudioManager& audioManager);
    ~PVMainComponent ();

	void filenameComponentChanged (FilenameComponent*);
	void timerCallback ();
    void changeListenerCallback (ChangeBroadcaster* source);

    void paint (Graphics& g);
    void resized();
    void buttonClicked (Button* buttonThatWasClicked);
    void sliderValueChanged (Slider* sliderThatWasMoved);

private:
	AudioManager& audioManager;

	FilenameComponent fileChooser;
	WaveformDisplay waveformDisplay;

    Label header_label1;
    GroupComponent details_group1;
    Label cpu_label1;
    TextButton settings_btn1;
    TextButton play_btn1;
    TextButton stop_btn1;
    Slider playback_pos_slider1;
    Label label;
    Slider vol_slider1;
    Label label2;
    Slider pitch_slider1;
    Label label3;
    Slider time_slider1;
    ToggleButton repeat_btn1;
    ToggleButton phaselock_btn1;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PVMainComponent)
};

#endif   // __JUCE_HEADER_8F8F37798A797554__

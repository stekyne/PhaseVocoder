/*
  ==============================================================================

  This is an automatically generated GUI class created by the Introjucer!

  Be careful when adding custom code to these files, as only the code within
  the "//[xyz]" and "//[/xyz]" sections will be retained when the file is loaded
  and re-saved.

  Created with Introjucer version: 3.1.1

  ------------------------------------------------------------------------------

  The Introjucer is part of the JUCE library - "Jules' Utility Class Extensions"
  Copyright 2004-13 by Raw Material Software Ltd.

  ==============================================================================
*/

#ifndef __JUCE_HEADER_8F8F37798A797554__
#define __JUCE_HEADER_8F8F37798A797554__

//[Headers]     -- You can add your own extra header files here --

#include "../JuceLibraryCode/JuceHeader.h"
#include "WaveformDisplayComponent.h"

//[/Headers]



//==============================================================================
/**
                                                                    //[Comments]
    An auto-generated component, created by the Jucer.

    Describe your class and how it works here!
                                                                    //[/Comments]
*/
class PVMainComponent  : public Component,
                         public FilenameComponentListener,
                         public Timer,
                         public ButtonListener,
                         public SliderListener
{
public:
    //==============================================================================
    PVMainComponent ();
    ~PVMainComponent();

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.

	void filenameComponentChanged (FilenameComponent*);
	void timerCallback ();

    //[/UserMethods]

    void paint (Graphics& g);
    void resized();
    void buttonClicked (Button* buttonThatWasClicked);
    void sliderValueChanged (Slider* sliderThatWasMoved);



private:
    //[UserVariables]   -- You can add your own custom variables in this section.

	AudioManager* audio_manager;

	ScopedPointer<AudioFormatManager> format_manager;
	ScopedPointer<FilenameComponent> fileChooser;
	ScopedPointer<WaveformDisplay> waveform_display;

    //[/UserVariables]

    //==============================================================================
    ScopedPointer<Label> header_label1;
    ScopedPointer<GroupComponent> details_group1;
    ScopedPointer<Label> cpu_label1;
    ScopedPointer<TextButton> settings_btn1;
    ScopedPointer<TextButton> play_btn1;
    ScopedPointer<TextButton> stop_btn1;
    ScopedPointer<Slider> playback_pos_slider1;
    ScopedPointer<Label> label;
    ScopedPointer<Slider> vol_slider1;
    ScopedPointer<Label> label2;
    ScopedPointer<Slider> pitch_slider1;
    ScopedPointer<Label> label3;
    ScopedPointer<Slider> time_slider1;
    ScopedPointer<ToggleButton> repeat_btn1;
    ScopedPointer<ToggleButton> phaselock_btn1;


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PVMainComponent)
};

//[EndFile] You can add extra defines here...
//[/EndFile]

#endif   // __JUCE_HEADER_8F8F37798A797554__

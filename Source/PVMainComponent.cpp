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

//[Headers] You can add your own extra header files here...
//[/Headers]

#include "PVMainComponent.h"


//[MiscUserDefs] You can add your own user definitions and misc code here...
//[/MiscUserDefs]

//==============================================================================
PVMainComponent::PVMainComponent (AudioManager* audioManager)
    :   audioManager (audioManager)
{
    addAndMakeVisible (header_label1 = new Label ("header_label1",
                                                  TRANS("Phase Vocoder")));
    header_label1->setFont (Font (22.00f, Font::bold));
    header_label1->setJustificationType (Justification::topLeft);
    header_label1->setEditable (false, false, false);
    header_label1->setColour (Label::textColourId, Colour (0xff414141));
    header_label1->setColour (TextEditor::textColourId, Colours::black);
    header_label1->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    addAndMakeVisible (details_group1 = new GroupComponent ("details_group1",
                                                            TRANS("Info")));
    details_group1->setColour (GroupComponent::outlineColourId, Colour (0xff727272));

    addAndMakeVisible (cpu_label1 = new Label ("cpu_label1",
                                               TRANS("CPU: 100%")));
    cpu_label1->setFont (Font ("Arial", 14.00f, Font::bold));
    cpu_label1->setJustificationType (Justification::centred);
    cpu_label1->setEditable (false, false, false);
    cpu_label1->setColour (Label::textColourId, Colour (0xff282828));
    cpu_label1->setColour (TextEditor::textColourId, Colours::black);
    cpu_label1->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    addAndMakeVisible (settings_btn1 = new TextButton ("settings_btn1"));
    settings_btn1->setTooltip (TRANS("Change audio and FFT settings here"));
    settings_btn1->setButtonText (TRANS("Audio Setup"));
    settings_btn1->addListener (this);
    settings_btn1->setColour (TextButton::buttonColourId, Colour (0xff727272));
    settings_btn1->setColour (TextButton::buttonOnColourId, Colour (0xff377a70));
    settings_btn1->setColour (TextButton::textColourOnId, Colours::white);
    settings_btn1->setColour (TextButton::textColourOffId, Colours::white);

    addAndMakeVisible (play_btn1 = new TextButton ("play_btn1"));
    play_btn1->setTooltip (TRANS("Play the sound file currently loaded, does nothing if no file is loaded"));
    play_btn1->setButtonText (TRANS("Play"));
    play_btn1->setConnectedEdges (Button::ConnectedOnRight);
    play_btn1->addListener (this);
    play_btn1->setColour (TextButton::buttonColourId, Colour (0xff3f9c48));
    play_btn1->setColour (TextButton::buttonOnColourId, Colour (0xffe6df64));

    addAndMakeVisible (stop_btn1 = new TextButton ("stop_btn1"));
    stop_btn1->setTooltip (TRANS("Stop playing the current sound file, does nothing if no file is loaded"));
    stop_btn1->setButtonText (TRANS("Stop"));
    stop_btn1->setConnectedEdges (Button::ConnectedOnLeft);
    stop_btn1->addListener (this);
    stop_btn1->setColour (TextButton::buttonColourId, Colour (0xffa44444));
    stop_btn1->setColour (TextButton::buttonOnColourId, Colour (0xffe6df64));

    addAndMakeVisible (playback_pos_slider1 = new Slider ("playback_pos_slider1"));
    playback_pos_slider1->setTooltip (TRANS("The current playback position of the sound file"));
    playback_pos_slider1->setRange (0, 1, 0);
    playback_pos_slider1->setSliderStyle (Slider::LinearHorizontal);
    playback_pos_slider1->setTextBoxStyle (Slider::TextBoxAbove, false, 90, 20);
    playback_pos_slider1->addListener (this);

    addAndMakeVisible (label = new Label ("new label",
                                          TRANS("Volume")));
    label->setFont (Font (15.00f, Font::plain));
    label->setJustificationType (Justification::centredLeft);
    label->setEditable (false, false, false);
    label->setColour (TextEditor::textColourId, Colours::black);
    label->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    addAndMakeVisible (vol_slider1 = new Slider ("vol_slider1"));
    vol_slider1->setTooltip (TRANS("Adjusts the volume of the loaded sound file"));
    vol_slider1->setRange (0, 1, 0.001);
    vol_slider1->setSliderStyle (Slider::LinearHorizontal);
    vol_slider1->setTextBoxStyle (Slider::TextBoxRight, false, 40, 15);
    vol_slider1->addListener (this);

    addAndMakeVisible (label2 = new Label ("new label",
                                           TRANS("Pitch")));
    label2->setFont (Font (15.00f, Font::plain));
    label2->setJustificationType (Justification::centredLeft);
    label2->setEditable (false, false, false);
    label2->setColour (TextEditor::textColourId, Colours::black);
    label2->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    addAndMakeVisible (pitch_slider1 = new Slider ("pitch_slider1"));
    pitch_slider1->setTooltip (TRANS("Adjusts the pitch of the loaded sound file, the setting is a ratio so 1:1 is the original playback speed"));
    pitch_slider1->setRange (0.5, 2, 0.001);
    pitch_slider1->setSliderStyle (Slider::LinearHorizontal);
    pitch_slider1->setTextBoxStyle (Slider::TextBoxRight, false, 40, 15);
    pitch_slider1->addListener (this);

    addAndMakeVisible (label3 = new Label ("new label",
                                           TRANS("Time")));
    label3->setFont (Font (15.00f, Font::plain));
    label3->setJustificationType (Justification::centredLeft);
    label3->setEditable (false, false, false);
    label3->setColour (TextEditor::textColourId, Colours::black);
    label3->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    addAndMakeVisible (time_slider1 = new Slider ("time_slider1"));
    time_slider1->setTooltip (TRANS("Adjusts the length of the loaded sound file, the setting is a ratio so 1:1 is the original playback length"));
    time_slider1->setRange (0.001, 2, 0.001);
    time_slider1->setSliderStyle (Slider::LinearHorizontal);
    time_slider1->setTextBoxStyle (Slider::TextBoxRight, false, 40, 15);
    time_slider1->addListener (this);

    addAndMakeVisible (repeat_btn1 = new ToggleButton ("repeat_btn1"));
    repeat_btn1->setTooltip (TRANS("Click to repeatedly play the audio file, when the play button is pressed"));
    repeat_btn1->setButtonText (TRANS("Repeat"));
    repeat_btn1->addListener (this);

    addAndMakeVisible (phaselock_btn1 = new ToggleButton ("phaselock_btn1"));
    phaselock_btn1->setTooltip (TRANS("To turn phase locking on or off for the algorithm"));
    phaselock_btn1->setButtonText (TRANS("Phase Lock"));
    phaselock_btn1->addListener (this);
    phaselock_btn1->setToggleState (true, dontSendNotification);


    //[UserPreSize]

    addAndMakeVisible (waveformDisplay = new WaveformDisplay (audioManager));
	addAndMakeVisible (fileChooser
		= new FilenameComponent("Audio File",
                                File::nonexistent,
                                true, false, false,
								audioManager->getFormatManager ()->getWildcardForAllFormats (),
                                String::empty,
                                "(Choose a WAV or AIFF file to play)"));
    fileChooser->addListener (this);
    fileChooser->setBrowseButtonText ("Browse");

    //[/UserPreSize]

    setSize (620, 400);


    //[Constructor] You can add your own custom stuff here..

	vol_slider1->setValue (1.0);
	pitch_slider1->setValue (1.0);
	time_slider1->setValue (1.0);
	startTimer (1000 / 30);
    audioManager->addChangeListener (this);

    //[/Constructor]
}

PVMainComponent::~PVMainComponent()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
	stopTimer ();
	fileChooser->removeListener (this);

    //[/Destructor_pre]

    header_label1 = nullptr;
    details_group1 = nullptr;
    cpu_label1 = nullptr;
    settings_btn1 = nullptr;
    play_btn1 = nullptr;
    stop_btn1 = nullptr;
    playback_pos_slider1 = nullptr;
    label = nullptr;
    vol_slider1 = nullptr;
    label2 = nullptr;
    pitch_slider1 = nullptr;
    label3 = nullptr;
    time_slider1 = nullptr;
    repeat_btn1 = nullptr;
    phaselock_btn1 = nullptr;


    //[Destructor]. You can add your own custom destruction code here..
    //[/Destructor]
}

//==============================================================================
void PVMainComponent::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    g.fillAll (Colour (0xff3f3f3f));

    g.setColour (Colour (0xff8fbcc7));
    g.fillRoundedRectangle (10.0f, 10.0f, static_cast<float> (getWidth() - 20), static_cast<float> (getHeight() - 20), 10.000f);

    g.setColour (Colour (0xff8e8e8e));
    g.drawRoundedRectangle (10.0f, 10.0f, static_cast<float> (getWidth() - 20), static_cast<float> (getHeight() - 20), 10.000f, 6.000f);

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void PVMainComponent::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    header_label1->setBounds (16, 16, 160, 24);
    details_group1->setBounds (448, 16, 136, 88);
    cpu_label1->setBounds (456, 66, 120, 24);
    settings_btn1->setBounds (477, 35, 80, 24);
    play_btn1->setBounds (22, 97, 96, 24);
    stop_btn1->setBounds (118, 97, 96, 24);
    playback_pos_slider1->setBounds (28, 46, 176, 50);
    label->setBounds (228, 31, 56, 24);
    vol_slider1->setBounds (284, 31, 152, 24);
    label2->setBounds (228, 55, 56, 24);
    pitch_slider1->setBounds (284, 55, 152, 24);
    label3->setBounds (228, 79, 56, 24);
    time_slider1->setBounds (284, 79, 152, 24);
    repeat_btn1->setBounds (234, 103, 72, 24);
    phaselock_btn1->setBounds (312, 104, 96, 24);
    //[UserResized] Add your own custom resize handling here..

	fileChooser->setBounds (24, 137, getWidth()-48, 24);
    waveformDisplay->setBounds (24, 165, getWidth () - 48, getHeight () - 165 - 24);

    //[/UserResized]
}

void PVMainComponent::buttonClicked (Button* buttonThatWasClicked)
{
    //[UserbuttonClicked_Pre]
    //[/UserbuttonClicked_Pre]

    if (buttonThatWasClicked == settings_btn1)
    {
        //[UserButtonCode_settings_btn1] -- add your button handler code here..

		DialogWindow::showModalDialog ("Audio Device Settings",
									    audioManager->getSelector(),
										this, Colours::grey,
										true);

        //[/UserButtonCode_settings_btn1]
    }
    else if (buttonThatWasClicked == play_btn1)
    {
        //[UserButtonCode_play_btn1] -- add your button handler code here..

		audioManager->play ();

        //[/UserButtonCode_play_btn1]
    }
    else if (buttonThatWasClicked == stop_btn1)
    {
        //[UserButtonCode_stop_btn1] -- add your button handler code here..

		audioManager->stop ();

        //[/UserButtonCode_stop_btn1]
    }
    else if (buttonThatWasClicked == repeat_btn1)
    {
        //[UserButtonCode_repeat_btn1] -- add your button handler code here..

		audioManager->setLooping (repeat_btn1->getToggleState ());

        //[/UserButtonCode_repeat_btn1]
    }
    else if (buttonThatWasClicked == phaselock_btn1)
    {
        //[UserButtonCode_phaselock_btn1] -- add your button handler code here..

		audioManager->setPhaseLock (phaselock_btn1->getToggleState ());

        //[/UserButtonCode_phaselock_btn1]
    }

    //[UserbuttonClicked_Post]
    //[/UserbuttonClicked_Post]
}

void PVMainComponent::sliderValueChanged (Slider* sliderThatWasMoved)
{
    //[UsersliderValueChanged_Pre]
    //[/UsersliderValueChanged_Pre]

    if (sliderThatWasMoved == playback_pos_slider1)
    {
        //[UserSliderCode_playback_pos_slider1] -- add your slider handling code here..

		audioManager->setPosition ((float)playback_pos_slider1->getValue());

        //[/UserSliderCode_playback_pos_slider1]
    }
    else if (sliderThatWasMoved == vol_slider1)
    {
        //[UserSliderCode_vol_slider1] -- add your slider handling code here..

		audioManager->setGain ((float)vol_slider1->getValue());

        //[/UserSliderCode_vol_slider1]
    }
    else if (sliderThatWasMoved == pitch_slider1)
    {
        //[UserSliderCode_pitch_slider1] -- add your slider handling code here..

		audioManager->setPitch ((float)pitch_slider1->getValue());

        //[/UserSliderCode_pitch_slider1]
    }
    else if (sliderThatWasMoved == time_slider1)
    {
        //[UserSliderCode_time_slider1] -- add your slider handling code here..

		audioManager->setTimeScale ((float)time_slider1->getValue());

        //[/UserSliderCode_time_slider1]
    }

    //[UsersliderValueChanged_Post]
    //[/UsersliderValueChanged_Post]
}



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...

void PVMainComponent::timerCallback ()
{
	const double position = audioManager->getPlaybackPosition ();

	playback_pos_slider1->setValue (position);
    waveformDisplay->setPosition (position);

	cpu_label1->setText ("CPU: " + String (audioManager->getCPU (), 2)
						 + "%", NotificationType::sendNotification);
}

void PVMainComponent::filenameComponentChanged (FilenameComponent* filename)
{
    if (audioManager->setFileSource (fileChooser->getCurrentFile ()))
    {
        playback_pos_slider1->setRange (0.0f, audioManager->getPlaybackLength ());
        playback_pos_slider1->setValue (0.0f);
    }
}

void PVMainComponent::changeListenerCallback (ChangeBroadcaster* source)
{
    // TODO reload audio file
}

//[/MiscUserCode]


//==============================================================================
#if 0
/*  -- Introjucer information section --

    This is where the Introjucer stores the metadata that describe this GUI layout, so
    make changes in here at your peril!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="PVMainComponent" componentName=""
                 parentClasses="public Component, public FilenameComponentListener, public Timer"
                 constructorParams="" variableInitialisers="" snapPixels="8" snapActive="1"
                 snapShown="0" overlayOpacity="0.330" fixedSize="1" initialWidth="620"
                 initialHeight="400">
  <BACKGROUND backgroundColour="ff3f3f3f">
    <ROUNDRECT pos="10 10 20M 20M" cornerSize="10" fill="solid: ff8fbcc7" hasStroke="1"
               stroke="6, mitered, butt" strokeColour="solid: ff8e8e8e"/>
  </BACKGROUND>
  <LABEL name="header_label1" id="28699550f0deff74" memberName="header_label1"
         virtualName="" explicitFocusOrder="0" pos="16 16 160 24" textCol="ff414141"
         edTextCol="ff000000" edBkgCol="0" labelText="Phase Vocoder" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default font"
         fontsize="22" bold="1" italic="0" justification="9"/>
  <GROUPCOMPONENT name="details_group1" id="3e7f92702131eb5b" memberName="details_group1"
                  virtualName="" explicitFocusOrder="0" pos="448 16 136 88" outlinecol="ff727272"
                  title="Info"/>
  <LABEL name="cpu_label1" id="acd860a26c89e06e" memberName="cpu_label1"
         virtualName="" explicitFocusOrder="0" pos="456 66 120 24" textCol="ff282828"
         edTextCol="ff000000" edBkgCol="0" labelText="CPU: 100%" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Arial"
         fontsize="14" bold="1" italic="0" justification="36"/>
  <TEXTBUTTON name="settings_btn1" id="55c5c1a127c535c5" memberName="settings_btn1"
              virtualName="" explicitFocusOrder="0" pos="477 35 80 24" tooltip="Change audio and FFT settings here"
              bgColOff="ff727272" bgColOn="ff377a70" textCol="ffffffff" textColOn="ffffffff"
              buttonText="Audio Setup" connectedEdges="0" needsCallback="1"
              radioGroupId="0"/>
  <TEXTBUTTON name="play_btn1" id="9f24d6da218d1bdd" memberName="play_btn1"
              virtualName="" explicitFocusOrder="0" pos="22 97 96 24" tooltip="Play the sound file currently loaded, does nothing if no file is loaded"
              bgColOff="ff3f9c48" bgColOn="ffe6df64" buttonText="Play" connectedEdges="2"
              needsCallback="1" radioGroupId="0"/>
  <TEXTBUTTON name="stop_btn1" id="20ed2d1c372c371f" memberName="stop_btn1"
              virtualName="" explicitFocusOrder="0" pos="118 97 96 24" tooltip="Stop playing the current sound file, does nothing if no file is loaded"
              bgColOff="ffa44444" bgColOn="ffe6df64" buttonText="Stop" connectedEdges="1"
              needsCallback="1" radioGroupId="0"/>
  <SLIDER name="playback_pos_slider1" id="ce98970d98be8788" memberName="playback_pos_slider1"
          virtualName="" explicitFocusOrder="0" pos="28 46 176 50" tooltip="The current playback position of the sound file"
          min="0" max="1" int="0" style="LinearHorizontal" textBoxPos="TextBoxAbove"
          textBoxEditable="1" textBoxWidth="90" textBoxHeight="20" skewFactor="1"/>
  <LABEL name="new label" id="5386cff0b711d6f6" memberName="label" virtualName=""
         explicitFocusOrder="0" pos="228 31 56 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Volume" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="15"
         bold="0" italic="0" justification="33"/>
  <SLIDER name="vol_slider1" id="ca5a8de54f505942" memberName="vol_slider1"
          virtualName="" explicitFocusOrder="0" pos="284 31 152 24" tooltip="Adjusts the volume of the loaded sound file"
          min="0" max="1" int="0.001" style="LinearHorizontal" textBoxPos="TextBoxRight"
          textBoxEditable="1" textBoxWidth="40" textBoxHeight="15" skewFactor="1"/>
  <LABEL name="new label" id="22e0fd6a70b6cfcf" memberName="label2" virtualName=""
         explicitFocusOrder="0" pos="228 55 56 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Pitch" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="15"
         bold="0" italic="0" justification="33"/>
  <SLIDER name="pitch_slider1" id="7b618897c61e0d45" memberName="pitch_slider1"
          virtualName="" explicitFocusOrder="0" pos="284 55 152 24" tooltip="Adjusts the pitch of the loaded sound file, the setting is a ratio so 1:1 is the original playback speed"
          min="0.5" max="2" int="0.001" style="LinearHorizontal" textBoxPos="TextBoxRight"
          textBoxEditable="1" textBoxWidth="40" textBoxHeight="15" skewFactor="1"/>
  <LABEL name="new label" id="3541d0c9f1841aa8" memberName="label3" virtualName=""
         explicitFocusOrder="0" pos="228 79 56 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Time" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="15"
         bold="0" italic="0" justification="33"/>
  <SLIDER name="time_slider1" id="c3f3f47aeae3206" memberName="time_slider1"
          virtualName="" explicitFocusOrder="0" pos="284 79 152 24" tooltip="Adjusts the length of the loaded sound file, the setting is a ratio so 1:1 is the original playback length"
          min="0.001" max="2" int="0.001" style="LinearHorizontal" textBoxPos="TextBoxRight"
          textBoxEditable="1" textBoxWidth="40" textBoxHeight="15" skewFactor="1"/>
  <TOGGLEBUTTON name="repeat_btn1" id="c0abcfb6168189be" memberName="repeat_btn1"
                virtualName="" explicitFocusOrder="0" pos="234 103 72 24" tooltip="Click to repeatedly play the audio file, when the play button is pressed"
                buttonText="Repeat" connectedEdges="0" needsCallback="1" radioGroupId="0"
                state="0"/>
  <TOGGLEBUTTON name="phaselock_btn1" id="93f88d8b41150d18" memberName="phaselock_btn1"
                virtualName="" explicitFocusOrder="0" pos="312 104 96 24" tooltip="To turn phase locking on or off for the algorithm"
                buttonText="Phase Lock" connectedEdges="0" needsCallback="1"
                radioGroupId="0" state="1"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif


//[EndFile] You can add extra defines here...
//[/EndFile]

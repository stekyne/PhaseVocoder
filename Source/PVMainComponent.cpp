#include "PVMainComponent.h"

PVMainComponent::PVMainComponent (AudioManager& audioManager_) :
	audioManager (audioManager_),
	header_label1 ("header_label1", "Phase Vocoder"),
	details_group1 ("details_group1", "Info"),
	cpu_label1 ("cpu_label1", "CPU: 100%"),
	settings_btn1 ("settings_btn1"),
	play_btn1 ("play_btn1"),
	stop_btn1 ("stop_btn1"),
	playback_pos_slider1 ("playback_pos_slider1"),
	label ("label", "Volume"),
	vol_slider1 ("vol_slider1"),
	label2 ("new label", "Pitch"),
	pitch_slider1 ("pitch_slider1"),
	label3 ("new label", "Time"),
	time_slider1 ("time_slider1"),
	repeat_btn1 ("repeat_btn1"),
	phaselock_btn1 ("phaselock_btn1"),
	waveformDisplay (audioManager_),
	fileChooser ("Audio File",
		File::nonexistent,
		true, false, false,
		audioManager_.getFormatManager ().getWildcardForAllFormats (),
		String::empty,
		"(Choose a WAV or AIFF file to play)")
{
	setSize (620, 400);

    header_label1.setFont (Font (22.00f, Font::bold));
    header_label1.setJustificationType (Justification::topLeft);
    header_label1.setEditable (false, false, false);
    header_label1.setColour (Label::textColourId, Colour (0xff414141));
    header_label1.setColour (TextEditor::textColourId, Colours::black);
    header_label1.setColour (TextEditor::backgroundColourId, Colour (0x00000000));

	addAndMakeVisible (header_label1);

    details_group1.setColour (GroupComponent::outlineColourId, Colour (0xff727272));
	addAndMakeVisible (details_group1);

    cpu_label1.setFont (Font ("Arial", 14.00f, Font::bold));
    cpu_label1.setJustificationType (Justification::centred);
    cpu_label1.setEditable (false, false, false);
    cpu_label1.setColour (Label::textColourId, Colour (0xff282828));
    cpu_label1.setColour (TextEditor::textColourId, Colours::black);
    cpu_label1.setColour (TextEditor::backgroundColourId, Colour (0x00000000));

	addAndMakeVisible (cpu_label1);

    settings_btn1.setTooltip ("Change audio and FFT settings here");
    settings_btn1.setButtonText ("Audio Setup");
    settings_btn1.addListener (this);
    settings_btn1.setColour (TextButton::buttonColourId, Colour (0xff727272));
    settings_btn1.setColour (TextButton::buttonOnColourId, Colour (0xff377a70));
    settings_btn1.setColour (TextButton::textColourOnId, Colours::white);
    settings_btn1.setColour (TextButton::textColourOffId, Colours::white);

	addAndMakeVisible (settings_btn1);
    
    play_btn1.setTooltip ("Play the sound file currently loaded, does nothing if no file is loaded");
    play_btn1.setButtonText ("Play");
    play_btn1.setConnectedEdges (Button::ConnectedOnRight);
    play_btn1.addListener (this);
    play_btn1.setColour (TextButton::buttonColourId, Colour (0xff3f9c48));
    play_btn1.setColour (TextButton::buttonOnColourId, Colour (0xffe6df64));

	addAndMakeVisible (play_btn1);
    
    stop_btn1.setTooltip ("Stop playing the current sound file, does nothing if no file is loaded");
    stop_btn1.setButtonText ("Stop");
    stop_btn1.setConnectedEdges (Button::ConnectedOnLeft);
    stop_btn1.addListener (this);
    stop_btn1.setColour (TextButton::buttonColourId, Colour (0xffa44444));    
	stop_btn1.setColour (TextButton::buttonOnColourId, Colour (0xffe6df64));

	addAndMakeVisible (stop_btn1);

    playback_pos_slider1.setTooltip ("The current playback position of the sound file");
    playback_pos_slider1.setRange (0, 1, 0);
    playback_pos_slider1.setSliderStyle (Slider::LinearHorizontal);
    playback_pos_slider1.setTextBoxStyle (Slider::TextBoxAbove, false, 90, 20);
    playback_pos_slider1.addListener (this);

	addAndMakeVisible (playback_pos_slider1);

    label.setFont (Font (15.00f, Font::plain));
    label.setJustificationType (Justification::centredLeft);
    label.setEditable (false, false, false);
    label.setColour (TextEditor::textColourId, Colours::black);
    label.setColour (TextEditor::backgroundColourId, Colour (0x00000000));

	addAndMakeVisible (label);

    vol_slider1.setTooltip ("Adjusts the volume of the loaded sound file");
    vol_slider1.setRange (0, 1, 0.001);
    vol_slider1.setSliderStyle (Slider::LinearHorizontal);
    vol_slider1.setTextBoxStyle (Slider::TextBoxRight, false, 40, 15);
    vol_slider1.addListener (this);

	addAndMakeVisible (vol_slider1);

    label2.setFont (Font (15.00f, Font::plain));
    label2.setJustificationType (Justification::centredLeft);
    label2.setEditable (false, false, false);
    label2.setColour (TextEditor::textColourId, Colours::black);
    label2.setColour (TextEditor::backgroundColourId, Colour (0x00000000));

	addAndMakeVisible (label2);

    pitch_slider1.setTooltip ("Adjusts the pitch of the loaded sound file, the setting is a ratio so 1:1 is the original playback speed");
    pitch_slider1.setRange (0.5, 2, 0.001);
    pitch_slider1.setSliderStyle (Slider::LinearHorizontal);
    pitch_slider1.setTextBoxStyle (Slider::TextBoxRight, false, 40, 15);
    pitch_slider1.addListener (this);

	addAndMakeVisible (pitch_slider1);

    label3.setFont (Font (15.00f, Font::plain));
    label3.setJustificationType (Justification::centredLeft);
    label3.setEditable (false, false, false);
    label3.setColour (TextEditor::textColourId, Colours::black);
    label3.setColour (TextEditor::backgroundColourId, Colour (0x00000000));
	
	addAndMakeVisible (label3);
    
    time_slider1.setTooltip ("Adjusts the length of the loaded sound file, the setting is a ratio so 1:1 is the original playback length");
    time_slider1.setRange (0.001, 2, 0.001);
    time_slider1.setSliderStyle (Slider::LinearHorizontal);
    time_slider1.setTextBoxStyle (Slider::TextBoxRight, false, 40, 15);
    time_slider1.addListener (this);
	
	addAndMakeVisible (time_slider1);

    repeat_btn1.setTooltip ("Click to repeatedly play the audio file, when the play button is pressed");
    repeat_btn1.setButtonText ("Repeat");
    repeat_btn1.addListener (this);

	addAndMakeVisible (repeat_btn1);

    phaselock_btn1.setTooltip ("To turn phase locking on or off for the algorithm");
    phaselock_btn1.setButtonText ("Phase Lock");
    phaselock_btn1.addListener (this);
    phaselock_btn1.setToggleState (true, dontSendNotification);
	
	addAndMakeVisible (phaselock_btn1);
	
    addAndMakeVisible (waveformDisplay);

	addAndMakeVisible (fileChooser);
    fileChooser.addListener (this);
    fileChooser.setBrowseButtonText ("Browse");

	vol_slider1.setValue (1.0);
	pitch_slider1.setValue (1.0);
	time_slider1.setValue (1.0);
	
    audioManager.addChangeListener (this);

	startTimer (1000 / 30);
}

PVMainComponent::~PVMainComponent()
{
	stopTimer ();
	fileChooser.removeListener (this);
}

void PVMainComponent::paint (Graphics& g)
{
    g.fillAll (Colour (0xff3f3f3f));

    g.setColour (Colour (0xff8fbcc7));
    g.fillRoundedRectangle (10.0f, 10.0f, static_cast<float> (getWidth() - 20), static_cast<float> (getHeight() - 20), 10.000f);

    g.setColour (Colour (0xff8e8e8e));
    g.drawRoundedRectangle (10.0f, 10.0f, static_cast<float> (getWidth() - 20), static_cast<float> (getHeight() - 20), 10.000f, 6.000f);
    
}

void PVMainComponent::resized()
{
    header_label1.setBounds (16, 16, 160, 24);
    details_group1.setBounds (448, 16, 136, 88);
    cpu_label1.setBounds (456, 66, 120, 24);
    settings_btn1.setBounds (477, 35, 80, 24);
    play_btn1.setBounds (22, 97, 96, 24);
    stop_btn1.setBounds (118, 97, 96, 24);
    playback_pos_slider1.setBounds (28, 46, 176, 50);
    label.setBounds (228, 31, 56, 24);
    vol_slider1.setBounds (284, 31, 152, 24);
    label2.setBounds (228, 55, 56, 24);
    pitch_slider1.setBounds (284, 55, 152, 24);
    label3.setBounds (228, 79, 56, 24);
    time_slider1.setBounds (284, 79, 152, 24);
    repeat_btn1.setBounds (234, 103, 72, 24);
    phaselock_btn1.setBounds (312, 104, 96, 24);

	fileChooser.setBounds (24, 137, getWidth()-48, 24);
    waveformDisplay.setBounds (24, 165, getWidth () - 48, getHeight () - 165 - 24);
}

void PVMainComponent::buttonClicked (Button* buttonThatWasClicked)
{
    if (buttonThatWasClicked == &settings_btn1)
    {
		DialogWindow::showModalDialog ("Audio Device Settings",
									    audioManager.getSelector(),
										this, Colours::grey,
										true);
    }
    else if (buttonThatWasClicked == &play_btn1)
    {
		audioManager.play ();
    }
    else if (buttonThatWasClicked == &stop_btn1)
    {
		audioManager.stop ();
    }
    else if (buttonThatWasClicked == &repeat_btn1)
    {
		audioManager.setLooping (repeat_btn1.getToggleState ());
    }
    else if (buttonThatWasClicked == &phaselock_btn1)
    {
		audioManager.setPhaseLock (phaselock_btn1.getToggleState ());
    }
}

void PVMainComponent::sliderValueChanged (Slider* sliderThatWasMoved)
{
    if (sliderThatWasMoved == &playback_pos_slider1)
    {
		audioManager.setPosition ((float)playback_pos_slider1.getValue());
    }
    else if (sliderThatWasMoved == &vol_slider1)
    {
		audioManager.setGain ((float)vol_slider1.getValue());
    }
    else if (sliderThatWasMoved == &pitch_slider1)
    {
		audioManager.setPitch ((float)pitch_slider1.getValue());
    }
    else if (sliderThatWasMoved == &time_slider1)
    {
		audioManager.setTimeScale ((float)time_slider1.getValue());
    }
}

void PVMainComponent::timerCallback ()
{
	const double position = audioManager.getPlaybackPosition ();

	playback_pos_slider1.setValue (position);
    waveformDisplay.setPosition (position);

	cpu_label1.setText ("CPU: " + String (audioManager.getCPU (), 2)
						 + "%", NotificationType::sendNotification);
}

void PVMainComponent::filenameComponentChanged (FilenameComponent* /*filename*/)
{
    if (audioManager.setFileSource (fileChooser.getCurrentFile ()))
    {
        playback_pos_slider1.setRange (0.0f, audioManager.getPlaybackLength ());
        playback_pos_slider1.setValue (0.0f);
    }
}

void PVMainComponent::changeListenerCallback (ChangeBroadcaster* /*source*/)
{
    // TODO reload audio file
}

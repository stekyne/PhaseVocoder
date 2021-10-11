#include "PluginProcessor.h"
#include "PluginEditor.h"

PhaseVocoderAudioProcessorEditor::PhaseVocoderAudioProcessorEditor (PhaseVocoderAudioProcessor* p): 
	AudioProcessorEditor (*p), 
	processor (p),
	pitchAttachment (p->state, "Pitch", pitchSlider)
{
	addAndMakeVisible (pitchSlider);
	setSize (400, 300);
}

PhaseVocoderAudioProcessorEditor::~PhaseVocoderAudioProcessorEditor()
{
}

void PhaseVocoderAudioProcessorEditor::paint (juce::Graphics& g)
{
	g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void PhaseVocoderAudioProcessorEditor::resized()
{
	pitchSlider.setBounds (0, 0, 400, 30);
}

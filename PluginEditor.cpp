#include "PluginProcessor.h"
#include "PluginEditor.h"

PsolaAudioProcessorEditor::PsolaAudioProcessorEditor (PsolaAudioProcessor* p): 
	AudioProcessorEditor (*p), 
	processor (p),
	pitchAttachment (p->state, "Pitch", pitchSlider)
{
	addAndMakeVisible (pitchSlider);
	setSize (400, 300);
}

PsolaAudioProcessorEditor::~PsolaAudioProcessorEditor()
{
}

void PsolaAudioProcessorEditor::paint (juce::Graphics& g)
{
	g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void PsolaAudioProcessorEditor::resized()
{
	pitchSlider.setBounds (0, 0, 400, 30);
}

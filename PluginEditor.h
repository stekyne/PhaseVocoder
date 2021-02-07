#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include "PluginProcessor.h"

class PsolaAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    PsolaAudioProcessorEditor (PsolaAudioProcessor* procesor);
    ~PsolaAudioProcessorEditor();

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    PsolaAudioProcessor* processor;
	juce::Slider pitchSlider;
	juce::AudioProcessorValueTreeState::SliderAttachment pitchAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PsolaAudioProcessorEditor)
};

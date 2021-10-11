#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>

#include "PluginProcessor.h"

class PhaseVocoderAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    PhaseVocoderAudioProcessorEditor (PhaseVocoderAudioProcessor* procesor);
    ~PhaseVocoderAudioProcessorEditor();

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    PhaseVocoderAudioProcessor* processor;
	juce::Slider pitchSlider;
	juce::AudioProcessorValueTreeState::SliderAttachment pitchAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PhaseVocoderAudioProcessorEditor)
};

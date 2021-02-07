#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

#include "DSP/BlockCircularBuffer.h"
#include "DSP/PhaseVocoder.h"
#include "DSP/PitchShifter.h"
#include "DSP/PeakShifter.h"
#include <atomic>

class PsolaAudioProcessor  : public juce::AudioProcessor
{
public:
    PsolaAudioProcessor();
    ~PsolaAudioProcessor();

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect () const override;
    double getTailLengthSeconds() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

	juce::AudioProcessorValueTreeState state;

private:
	PitchShifter<float> pitchShifter;
	PeakShifter<float> peakShifter;
	std::atomic<float>* pitchParam = nullptr;

#ifdef DEBUG
    BlockCircularBuffer<float> history[2];
#endif

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PsolaAudioProcessor)
};

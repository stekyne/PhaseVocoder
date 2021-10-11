#include "PluginProcessor.h"
#include "PluginEditor.h"

PhaseVocoderAudioProcessor::PhaseVocoderAudioProcessor() :
	state (*this, nullptr, "Params", {
		std::make_unique<juce::AudioParameterFloat> ("Pitch", "Pitch Multiplier", 
			juce::NormalisableRange<float> (0.5f, 2.f), 1.0f)
	})
#ifndef JucePlugin_PreferredChannelConfigurations
       ,AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
	const auto window = pitchShifter.getLatencyInSamples ();

#ifdef DEBUG
    history[0].setSize (3 * window, true);
    history[1].setSize (3 * window, true);
#endif

	pitchParam = state.getRawParameterValue ("Pitch");

    // Must be set by the phasevocoder class
	setLatencySamples (window);
}

PhaseVocoderAudioProcessor::~PhaseVocoderAudioProcessor()
{
}

const juce::String PhaseVocoderAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool PhaseVocoderAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool PhaseVocoderAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool PhaseVocoderAudioProcessor::isMidiEffect () const
{
   #ifdef JucePlugin_IsMidiEffect
    return false;
   #else
    return false;
   #endif
}

double PhaseVocoderAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int PhaseVocoderAudioProcessor::getNumPrograms()
{
    return 1;
}

int PhaseVocoderAudioProcessor::getCurrentProgram()
{
    return 0;
}

void PhaseVocoderAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String PhaseVocoderAudioProcessor::getProgramName (int index)
{
    return {};
}

void PhaseVocoderAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

void PhaseVocoderAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
}

void PhaseVocoderAudioProcessor::releaseResources()
{
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool PhaseVocoderAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono() &&
        layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void PhaseVocoderAudioProcessor::processBlock (juce::AudioSampleBuffer& buffer, juce::MidiBuffer& midiMessages)
{
    const auto numSamples = buffer.getNumSamples ();
    const auto totalNumInputChannels  = getTotalNumInputChannels();
    const auto totalNumOutputChannels = getTotalNumOutputChannels();

    // Need 2 input and output channels at least, further channels will be ignored
    jassert (totalNumInputChannels >= 2);
    jassert (totalNumOutputChannels >= 2);

	// Clear unused output channels
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, numSamples);

	pitchShifter.setPitchRatio (pitchParam->load());
	pitchShifter.process (buffer.getWritePointer (0), numSamples);
	//peakShifter.process (buffer.getWritePointer (0), numSamples);

	buffer.copyFrom (1, 0, buffer.getWritePointer (0), numSamples);

#ifdef DEBUG
    // Store output in history buffer
    history[0].write (buffer.getReadPointer (0), numSamples);
#endif
}

bool PhaseVocoderAudioProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* PhaseVocoderAudioProcessor::createEditor()
{
    return new PhaseVocoderAudioProcessorEditor (this);
}

//==============================================================================
void PhaseVocoderAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{

}

void PhaseVocoderAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{

}

// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new PhaseVocoderAudioProcessor();
}

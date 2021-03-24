#include "PluginProcessor.h"
#include "PluginEditor.h"

PsolaAudioProcessor::PsolaAudioProcessor() :
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

PsolaAudioProcessor::~PsolaAudioProcessor()
{
}

const juce::String PsolaAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool PsolaAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool PsolaAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool PsolaAudioProcessor::isMidiEffect () const
{
   #ifdef JucePlugin_IsMidiEffect
    return false;
   #else
    return false;
   #endif
}

double PsolaAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int PsolaAudioProcessor::getNumPrograms()
{
    return 1;
}

int PsolaAudioProcessor::getCurrentProgram()
{
    return 0;
}

void PsolaAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String PsolaAudioProcessor::getProgramName (int index)
{
    return {};
}

void PsolaAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

void PsolaAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
}

void PsolaAudioProcessor::releaseResources()
{
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool PsolaAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void PsolaAudioProcessor::processBlock (juce::AudioSampleBuffer& buffer, juce::MidiBuffer& midiMessages)
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

    if (false)
    {
        static float phase = 0.f;
        static float phaseIncrem = 100.f * juce::MathConstants<float>::twoPi / (float)getSampleRate ();

        auto channels = buffer.getArrayOfWritePointers ();

        for (int i = 0; i < buffer.getNumSamples (); ++i)
        { 
			//channels[0][i] = channels[1][i] = 1.f;
            channels[0][i] = channels[1][i] = sinf (phase += phaseIncrem);
            phase = fmodf (phase, juce::MathConstants<float>::twoPi);
        }
    }

	pitchShifter.setPitchRatio (pitchParam->load());
	pitchShifter.process (buffer.getWritePointer (0), numSamples);
	//peakShifter.process (buffer.getWritePointer (0), numSamples);

	buffer.copyFrom (1, 0, buffer.getWritePointer (0), numSamples);

#ifdef DEBUG
    // Store output in history buffer
    history[0].write (buffer.getReadPointer (0), numSamples);
#endif
}

bool PsolaAudioProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* PsolaAudioProcessor::createEditor()
{
    return new PsolaAudioProcessorEditor (this);
}

//==============================================================================
void PsolaAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{

}

void PsolaAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{

}

// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new PsolaAudioProcessor();
}

/*
 ==============================================================================
 
 This file contains the basic framework code for a JUCE plugin processor.
 
 ==============================================================================
 */

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
TremoloCusterAudioProcessor::TremoloCusterAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
: AudioProcessor (BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
                  .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
#endif
                  .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
#endif
                  ),
apvts (*this, nullptr, "Parameters", createParameterLayout())
#endif
{
}

TremoloCusterAudioProcessor::~TremoloCusterAudioProcessor()
{
}

juce::AudioProcessorValueTreeState::ParameterLayout TremoloCusterAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;
    
    layout.add(std::make_unique<juce::AudioParameterFloat>("RATE", "Velocidad", juce::NormalisableRange<float>(0.1f, 20.0f, 0.1f, 0.5f), 5.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("DEPTH", "Profundidad", juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.8f));
    
    return layout;
}

//==============================================================================
const juce::String TremoloCusterAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool TremoloCusterAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool TremoloCusterAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool TremoloCusterAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double TremoloCusterAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int TremoloCusterAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
    // so this should be at least 1, even if you're not really implementing programs.
}

int TremoloCusterAudioProcessor::getCurrentProgram()
{
    return 0;
}

void TremoloCusterAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String TremoloCusterAudioProcessor::getProgramName (int index)
{
    return {};
}

void TremoloCusterAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void TremoloCusterAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Calcula cuánto debe avanzar la onda (fase) por cada muestra de audio.
    // Fórmula: (Frecuencia deseada / Frecuencia de muestreo) * 2 PI
    float currentRate = apvts.getRawParameterValue("RATE")->load();
    auto cyclesPerSample = currentRate / sampleRate;
    phaseDelta = cyclesPerSample * 2.0 * juce::MathConstants<float>::pi;
}

void TremoloCusterAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool TremoloCusterAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
#else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
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

void TremoloCusterAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    
    // Leemos los valores en vivo desde los sliders de la UI
    float currentRate = apvts.getRawParameterValue("RATE")->load();
    float currentDepth = apvts.getRawParameterValue("DEPTH")->load();
    
    // Recalculamos la velocidad en tiempo real
    auto cyclesPerSample = currentRate / getSampleRate();
    phaseDelta = cyclesPerSample * 2.0 * juce::MathConstants<float>::pi;
    
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());
    
    for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
    {
        float lfoValue = std::sin(currentPhase);
        float unipolarLFO = (lfoValue + 1.0f) * 0.5f;
        
        // Usamos currentDepth en lugar de la variable vieja 'depth'
        float currentGain = 1.0f - (currentDepth * unipolarLFO);
        
        for (int channel = 0; channel < totalNumInputChannels; ++channel)
        {
            auto* channelData = buffer.getWritePointer (channel);
            channelData[sample] *= currentGain;
        }
        
        currentPhase += phaseDelta;
        
        if (currentPhase >= 2.0f * juce::MathConstants<float>::pi)
            currentPhase -= 2.0f * juce::MathConstants<float>::pi;
    }
}

//==============================================================================
bool TremoloCusterAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* TremoloCusterAudioProcessor::createEditor()
{
    return new TremoloCusterAudioProcessorEditor (*this);
}

//==============================================================================
void TremoloCusterAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void TremoloCusterAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new TremoloCusterAudioProcessor();
}

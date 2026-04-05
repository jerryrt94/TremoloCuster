/*
 ==============================================================================
 
 This file contains the basic framework code for a JUCE plugin editor.
 
 ==============================================================================
 */

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
 */
class TremoloCusterAudioProcessorEditor  : public juce::AudioProcessorEditor
{
    public:
    TremoloCusterAudioProcessorEditor (TremoloCusterAudioProcessor&);
    ~TremoloCusterAudioProcessorEditor() override;
    
    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    
    private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    TremoloCusterAudioProcessor& audioProcessor;
    juce::Slider rateSlider;
    juce::Slider depthSlider;
    
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> rateAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> depthAttachment;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TremoloCusterAudioProcessorEditor)
};

/*
 ==============================================================================
 
 This file contains the basic framework code for a JUCE plugin editor.
 
 ==============================================================================
 */

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
TremoloCusterAudioProcessorEditor::TremoloCusterAudioProcessorEditor (TremoloCusterAudioProcessor& p)
: AudioProcessorEditor (&p), audioProcessor (p)
{
    // Configuramos el Slider de Velocidad
    rateSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    rateSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
    addAndMakeVisible(rateSlider);
    
    // Conectamos el Slider al parámetro "RATE"
    rateAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "RATE", rateSlider);
    
    // Configuramos el Slider de Profundidad
    depthSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    depthSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
    addAndMakeVisible(depthSlider);
    
    // Conectamos el Slider al parámetro "DEPTH"
    depthAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "DEPTH", depthSlider);
    
    // Tamaño de la ventana del plugin (Ancho, Alto)
    setSize (400, 300);
}

TremoloCusterAudioProcessorEditor::~TremoloCusterAudioProcessorEditor()
{
}

//==============================================================================
void TremoloCusterAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
    
}

void TremoloCusterAudioProcessorEditor::resized()
{
    rateSlider.setBounds(50, 50, 100, 100);
    depthSlider.setBounds(250, 50, 100, 100);
}

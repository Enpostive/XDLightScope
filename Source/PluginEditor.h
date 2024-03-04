/*
 ==============================================================================
 
 This file contains the basic framework code for a JUCE plugin editor.
 
 ==============================================================================
 */

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "ColouredScope.h"

//==============================================================================
/**
 */
class XDLightScopeAudioProcessorEditor  : public juce::AudioProcessorEditor, public juce::Timer
{
public:
 XDLightScopeAudioProcessorEditor (XDLightScopeAudioProcessor&);
 ~XDLightScopeAudioProcessorEditor() override;
 
 //==============================================================================
 void paint (juce::Graphics&) override;
 void resized() override;
 
 virtual void timerCallback() override;

private:
 // This reference is provided as a quick way for your editor to
 // access the processor object that created it.
 XDLightScopeAudioProcessor& audioProcessor;
 
 ColouredScope leftScope;
 ColouredScope rightScope;
 CircularBufferSource<decltype(audioProcessor.leftAudioData)> leftSource;
 CircularBufferSource<decltype(audioProcessor.rightAudioData)> rightSource;
 JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (XDLightScopeAudioProcessorEditor)
};

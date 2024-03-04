/*
 ==============================================================================
 
 This file contains the basic framework code for a JUCE plugin processor.
 
 ==============================================================================
 */

#pragma once

#include <JuceHeader.h>
#include "XDDSP/XDDSP.h"

//==============================================================================
/**
 */
class XDLightScopeAudioProcessor  : public juce::AudioProcessor
{
public:
 //==============================================================================
 XDLightScopeAudioProcessor();
 ~XDLightScopeAudioProcessor() override;
 
 //==============================================================================
 void prepareToPlay (double sampleRate, int samplesPerBlock) override;
 void releaseResources() override;
 
#ifndef JucePlugin_PreferredChannelConfigurations
 bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
#endif
 
 void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
 
 //==============================================================================
 juce::AudioProcessorEditor* createEditor() override;
 bool hasEditor() const override;
 
 //==============================================================================
 const juce::String getName() const override;
 
 bool acceptsMidi() const override;
 bool producesMidi() const override;
 bool isMidiEffect() const override;
 double getTailLengthSeconds() const override;
 
 //==============================================================================
 int getNumPrograms() override;
 int getCurrentProgram() override;
 void setCurrentProgram (int index) override;
 const juce::String getProgramName (int index) override;
 void changeProgramName (int index, const juce::String& newName) override;
 
 //==============================================================================
 void getStateInformation (juce::MemoryBlock& destData) override;
 void setStateInformation (const void* data, int sizeInBytes) override;
 
 XDDSP::DynamicCircularBuffer<float> leftAudioData;
 XDDSP::DynamicCircularBuffer<float> rightAudioData;
 XDDSP::DynamicCircularBuffer<float> bassAudioData;
 XDDSP::DynamicCircularBuffer<float> midAudioData;
 XDDSP::DynamicCircularBuffer<float> highAudioData;
 XDDSP::Parameters dspParam;
 std::mutex buffMutex;
 
 bool longWait {false};
 
 std::chrono::duration<double> maximumWaitOnMutex {0};

 static constexpr float LowXOver = 600.;
 static constexpr float HighXOver = 4000.;
 
private:
 
 std::vector<float> procBuffer;
 
 XDDSP::BiquadFilterCoefficients lowLPCoeff;
 XDDSP::BiquadFilterCoefficients highLPCoeff;
 XDDSP::BiquadFilterKernel lowLP;
 XDDSP::BiquadFilterKernel highLP;
 //==============================================================================
 JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (XDLightScopeAudioProcessor)
};

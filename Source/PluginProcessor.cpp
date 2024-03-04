/*
 ==============================================================================
 
 This file contains the basic framework code for a JUCE plugin processor.
 
 ==============================================================================
 */

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
XDLightScopeAudioProcessor::XDLightScopeAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
: AudioProcessor (BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
                  .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
#endif
                  .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
#endif
                  )
#endif
,
lowLPCoeff(dspParam),
highLPCoeff(dspParam)
{
 static constexpr int smplLength = 5*44100;

 lowLPCoeff.setLowPassFilter(LowXOver, 0.707);
 highLPCoeff.setLowPassFilter(HighXOver, 0.707);
 leftAudioData.setMaximumLength(smplLength);
 rightAudioData.setMaximumLength(smplLength);
 bassAudioData.setMaximumLength(smplLength);
 midAudioData.setMaximumLength(smplLength);
 highAudioData.setMaximumLength(smplLength);
}

XDLightScopeAudioProcessor::~XDLightScopeAudioProcessor()
{
}

//==============================================================================
const juce::String XDLightScopeAudioProcessor::getName() const
{
 return JucePlugin_Name;
}

bool XDLightScopeAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
 return true;
#else
 return false;
#endif
}

bool XDLightScopeAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
 return true;
#else
 return false;
#endif
}

bool XDLightScopeAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
 return true;
#else
 return false;
#endif
}

double XDLightScopeAudioProcessor::getTailLengthSeconds() const
{
 return 0.0;
}

int XDLightScopeAudioProcessor::getNumPrograms()
{
 return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
 // so this should be at least 1, even if you're not really implementing programs.
}

int XDLightScopeAudioProcessor::getCurrentProgram()
{
 return 0;
}

void XDLightScopeAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String XDLightScopeAudioProcessor::getProgramName (int index)
{
 return {};
}

void XDLightScopeAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void XDLightScopeAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
 dspParam.setSampleRate(sampleRate);
 dspParam.setBufferSize(samplesPerBlock);
 procBuffer.resize(samplesPerBlock);
}

void XDLightScopeAudioProcessor::releaseResources()
{
 // When playback stops, you can use this as an opportunity to free up any
 // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool XDLightScopeAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void XDLightScopeAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
 juce::ScopedNoDenormals noDenormals;
 auto totalNumInputChannels  = getTotalNumInputChannels();
 auto totalNumOutputChannels = getTotalNumOutputChannels();
 
 for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
  buffer.clear (i, 0, buffer.getNumSamples());
 
 float sumScale = 1./static_cast<float>(getTotalNumInputChannels());

 for (auto i = 0; i < buffer.getNumSamples(); ++i)
 {
  float sum = 0.;
  for (int c = 0; c < getTotalNumInputChannels(); ++c)
  {
   sum += buffer.getSample(c, i);
  }
  sum *= sumScale;
  procBuffer[i] = sum;
 }
 
 {
  std::lock_guard<std::mutex> lock(buffMutex);

  for (auto i = 0; i < buffer.getNumSamples(); ++i)
  {
   leftAudioData.tapIn(buffer.getSample(0, i));
   rightAudioData.tapIn(buffer.getSample(1, i));
   
   float sum = procBuffer[i];
   float lp = lowLP.process(lowLPCoeff, sum);
   bassAudioData.tapIn(lp);
   sum -= lp;
   lp = highLP.process(highLPCoeff, sum);
   midAudioData.tapIn(lp);
   sum -= lp;
   highAudioData.tapIn(sum);
  }
 }
}

//==============================================================================
bool XDLightScopeAudioProcessor::hasEditor() const
{
 return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* XDLightScopeAudioProcessor::createEditor()
{
 return new XDLightScopeAudioProcessorEditor (*this);
}

//==============================================================================
void XDLightScopeAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
 // You should use this method to store your parameters in the memory block.
 // You could do that either as raw data, or use the XML or ValueTree classes
 // as intermediaries to make it easy to save and load complex data.
}

void XDLightScopeAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
 // You should use this method to restore your parameters from this memory block,
 // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
 return new XDLightScopeAudioProcessor();
}

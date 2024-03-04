/*
 ==============================================================================
 
 This file contains the basic framework code for a JUCE plugin editor.
 
 ==============================================================================
 */

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
XDLightScopeAudioProcessorEditor::XDLightScopeAudioProcessorEditor (XDLightScopeAudioProcessor& p)
: AudioProcessorEditor (&p), audioProcessor (p),
leftSource(p.leftAudioData,
           p.bassAudioData,
           p.midAudioData,
           p.highAudioData),
rightSource(p.rightAudioData,
            p.bassAudioData,
            p.midAudioData,
            p.highAudioData)
{
 // Make sure that before the constructor has finished, you've set the
 // editor's size to whatever you need it to be.
 addAndMakeVisible(leftScope);
 leftScope.setBounds(0, 0, 400, 80);
 leftScope.setBufferedToImage(true);
 leftScope.reverse = true;
 leftScope.source = &leftSource;
 leftScope.strokeEnable = true;
 leftScope.centreEnable = true;
 leftScope.centreLineColour = juce::Colours::black;
 leftSource.setWindowSize(66300);

 addAndMakeVisible(rightScope);
 rightScope.setBounds(0, 80, 400, 80);
 rightScope.setBufferedToImage(true);
 rightScope.reverse = true;
 rightScope.source = &rightSource;
 rightScope.strokeEnable = true;
 rightScope.centreEnable = true;
 rightScope.centreLineColour = juce::Colours::black;
 rightSource.setWindowSize(66300);

 setSize (400, 160);
 startTimerHz(30);
}

XDLightScopeAudioProcessorEditor::~XDLightScopeAudioProcessorEditor()
{
}

void XDLightScopeAudioProcessorEditor::timerCallback()
{
 double waitTime;
 {
  std::lock_guard<std::mutex> lock(audioProcessor.buffMutex);
  leftScope.update();
  rightScope.update();
  waitTime = audioProcessor.maximumWaitOnMutex.count();
  audioProcessor.maximumWaitOnMutex = audioProcessor.maximumWaitOnMutex.zero();
 }

/* waitDisplay.setText(juce::String(waitTime, 5) + "s" +
                     (audioProcessor.longWait ? " longwait!" : ""),
                     juce::NotificationType::dontSendNotification);
 */
 leftScope.repaint();
 rightScope.repaint();
}


//==============================================================================
void XDLightScopeAudioProcessorEditor::paint (juce::Graphics& g)
{
 // (Our component is opaque, so we must completely fill the background with a solid colour)
// g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void XDLightScopeAudioProcessorEditor::resized()
{
 // This is generally where you'll want to lay out the positions of any
 // subcomponents in your editor..
}

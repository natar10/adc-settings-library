/*
  ==============================================================================

   This file is part of the JUCE tutorials.
   Copyright (c) 2020 - Raw Material Software Limited

   The code included in this file is provided under the terms of the ISC license
   http://www.isc.org/downloads/software-support-policy/isc-license. Permission
   To use, copy, modify, and/or distribute this software for any purpose with or
   without fee is hereby granted provided that the above copyright notice and
   this permission notice appear in all copies.

   THE SOFTWARE IS PROVIDED "AS IS" WITHOUT ANY WARRANTY, AND ALL WARRANTIES,
   WHETHER EXPRESSED OR IMPLIED, INCLUDING MERCHANTABILITY AND FITNESS FOR
   PURPOSE, ARE DISCLAIMED.

  ==============================================================================
*/

/*******************************************************************************
 The block below describes the properties of this PIP. A PIP is a short snippet
 of code that can be read by the Projucer and used to generate a JUCE project.

 BEGIN_JUCE_PIP_METADATA

 name:             AudioProcessorValueTreeStateTutorialTest
 version:          1.0.0
 vendor:           DNA
 website:          http://juce.com
 description:      Explores the audio processor value tree state.

 dependencies:     juce_audio_basics, juce_audio_devices, juce_audio_formats,
                   juce_audio_plugin_client, juce_audio_processors,
                   juce_audio_utils, juce_core, juce_data_structures,
                   juce_events, juce_graphics, juce_gui_basics, juce_gui_extra
 exporters:        xcode_mac, vs2019, linux_make

 type:             AudioProcessor
 mainClass:        TutorialProcessor

 useLocalCopy:     1

 END_JUCE_PIP_METADATA

*******************************************************************************/

#pragma once

//==============================================================================

#include "PluginComponent.h"
#include "CloudComponent.h"

#include <JuceHeader.h>
#include <memory>

struct XMLSetting
{
    juce::String id;
    juce::String project;
};

class GenericEditor : public juce::AudioProcessorEditor
{
  public:
    typedef juce::AudioProcessorValueTreeState::SliderAttachment SliderAttachment;
    typedef juce::AudioProcessorValueTreeState::ButtonAttachment ButtonAttachment;

    GenericEditor(juce::AudioProcessor& parent, juce::AudioProcessorValueTreeState& vts, juce::ValueTree& tr);

    virtual void resized() override;
    virtual void paint(juce::Graphics& g) override;

  private:
    juce::AudioProcessorValueTreeState& valueTreeState;
    juce::ValueTree& tree;
    juce::TextButton showCloud;
    std::unique_ptr<CloudComponent> cloud;
    std::unique_ptr<PluginComponent> plugin;
};

//==============================================================================
class TutorialProcessor : public juce::AudioProcessor
{
  public:
    TutorialProcessor(bool isUserLoggedIn);

    virtual void prepareToPlay(double, int) override;
    void releaseResources() override;
    void processBlock(juce::AudioSampleBuffer& buffer, juce::MidiBuffer&) override;
    void exportValueTreeToXML();
    void loadXMLfromFile();
    virtual juce::AudioProcessorEditor* createEditor() override;
    virtual bool hasEditor() const override;
    const juce::String getName() const override;
    virtual bool acceptsMidi() const override;
    bool producesMidi() const override;
    double getTailLengthSeconds() const override;

    virtual int getNumPrograms() override;
    virtual int getCurrentProgram() override;
    virtual void setCurrentProgram(int) override;
    virtual const juce::String getProgramName(int) override;
    virtual void changeProgramName(int, const juce::String&) override;
    virtual void getStateInformation(juce::MemoryBlock& destData) override;
    virtual void setStateInformation(const void* data, int sizeInBytes) override;

  private:
    //==============================================================================
    juce::AudioProcessorValueTreeState parameters;
    juce::ValueTree tree{"main"};
    float previousGain; // [1]
    bool isUserActive;

    std::atomic<float>* phaseParameter = nullptr;
    std::atomic<float>* gainParameter = nullptr;
    juce::File desktopDir = File::getSpecialLocation(File::userDesktopDirectory);

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TutorialProcessor)
};

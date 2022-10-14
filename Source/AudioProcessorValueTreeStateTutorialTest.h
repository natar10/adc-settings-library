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

class XMLSetting {
  public:
    juce::String id;
    juce::String project;
};

class GenericEditor : public juce::AudioProcessorEditor
{
public:
    enum
    {
        paramControlHeight = 40,
        paramLabelWidth    = 80,
        paramSliderWidth   = 300
    };

    typedef juce::AudioProcessorValueTreeState::SliderAttachment SliderAttachment;
    typedef juce::AudioProcessorValueTreeState::ButtonAttachment ButtonAttachment;

    GenericEditor (juce::AudioProcessor& parent, juce::AudioProcessorValueTreeState& vts, juce::ValueTree& tr)
        : AudioProcessorEditor (parent),
          valueTreeState (vts), tree (tr)
    {
        addAndMakeVisible(cloud);
        addAndMakeVisible (plugin);
        
        showCloud.setButtonText("Serverless Cloud Library");
        addAndMakeVisible (showCloud);

        setSize (500, 600);
    }

    void resized() override
    {
        auto area = getLocalBounds();
        auto headerHeight = area.removeFromTop (36);
        auto fromTop = area.removeFromTop (200);
        showCloud.setBounds (headerHeight);
        cloud->setBounds (fromTop.removeFromLeft (200));
        plugin->setBounds (fromTop);
    }

    void paint (juce::Graphics& g) override
    {
        g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
    }

private:
    juce::AudioProcessorValueTreeState& valueTreeState;
    juce::ValueTree& tree;
    juce::TextButton showCloud;
    CloudComponent* cloud = new CloudComponent(valueTreeState, tree);
    PluginComponent* plugin = new PluginComponent(valueTreeState, tree);
};



//==============================================================================
class TutorialProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    TutorialProcessor()
        : parameters (*this, nullptr, juce::Identifier ("APVTSTutorial"),
                      {
                          std::make_unique<juce::AudioParameterFloat> ("gain",            // parameterID
                                                                       "Gain",            // parameter name
                                                                       0.0f,              // minimum value
                                                                       1.0f,              // maximum value
                                                                       0.5f),             // default value
                          std::make_unique<juce::AudioParameterBool> ("invertPhase",      // parameterID
                                                                      "Invert Phase",     // parameter name
                                                                      false)              // default value
                      })
    {
        phaseParameter = parameters.getRawParameterValue ("invertPhase");
        gainParameter  = parameters.getRawParameterValue ("gain");
    }

    //==============================================================================
    void prepareToPlay (double, int) override
    {
        auto phase = *phaseParameter < 0.5f ? 1.0f : -1.0f;
        previousGain = *gainParameter * phase;
    }
    
    void releaseResources() override {}

    void processBlock (juce::AudioSampleBuffer& buffer, juce::MidiBuffer&) override
    {
        auto phase = *phaseParameter < 0.5f ? 1.0f : -1.0f;
        auto currentGain = *gainParameter * phase;

        if (currentGain == previousGain)
        {
            buffer.applyGain (currentGain);
        }
        else
        {
            buffer.applyGainRamp (0, buffer.getNumSamples(), previousGain, currentGain);
            previousGain = currentGain;
        }
    }
    
    void exportValueTreeToXML()
    {
        File newPreset (desktopDir.getFullPathName() + "/saveData.xml");
        
        if (!newPreset.exists())
            newPreset.create();

        std::unique_ptr<juce::XmlElement> vtXML (tree.createXml());
        if(!(vtXML -> writeTo(newPreset)))
            DBG("FAIL");
        
    }

    void loadXMLfromFile()
    {
        File readFrom(desktopDir.getChildFile ("saveData.xml"));
        if(readFrom.existsAsFile())
        {
            XmlDocument xmlDoc (readFrom);
            if (auto mainElement = xmlDoc.getDocumentElement())
                tree = tree.fromXml(*mainElement);
        }
    }

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override          { return new GenericEditor (*this, parameters, tree); }
    bool hasEditor() const override                              { return true; }

    //==============================================================================
    const juce::String getName() const override                  { return "APVTS Tutorial"; }
    bool acceptsMidi() const override                            { return false; }
    bool producesMidi() const override                           { return false; }
    double getTailLengthSeconds() const override                 { return 0; }

    //==============================================================================
    int getNumPrograms() override                                { return 1; }
    int getCurrentProgram() override                             { return 0; }
    void setCurrentProgram (int) override                        {}
    const juce::String getProgramName (int) override             { return {}; }
    void changeProgramName (int, const juce::String&) override   {}

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override
    {
        auto state = parameters.copyState();
        std::unique_ptr<juce::XmlElement> xml (state.createXml());
        copyXmlToBinary (*xml, destData);
        if(tree.hasProperty("accessToken"))
            exportValueTreeToXML();
    }

    void setStateInformation (const void* data, int sizeInBytes) override
    {
        std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));
        if (xmlState.get() != nullptr)
            if (xmlState->hasTagName (parameters.state.getType()))
                parameters.replaceState (juce::ValueTree::fromXml (*xmlState));
                loadXMLfromFile();
    }

private:
    //==============================================================================
    juce::AudioProcessorValueTreeState parameters;
    juce::ValueTree tree{"main"};
    float previousGain; // [1]

    std::atomic<float>* phaseParameter = nullptr;
    std::atomic<float>* gainParameter  = nullptr;
    juce::File desktopDir = File::getSpecialLocation(File::userDesktopDirectory);

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TutorialProcessor)
};

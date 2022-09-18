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

class PluginComponent : public juce::Component
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

    PluginComponent (juce::AudioProcessorValueTreeState& vts)
        : valueTreeState (vts)
    {
        gainLabel.setText ("Gain", juce::dontSendNotification);
        addAndMakeVisible (gainLabel);

        addAndMakeVisible (gainSlider);
        gainAttachment.reset (new SliderAttachment (valueTreeState, "gain", gainSlider));
        
        saveButton.setButtonText("Save to Cloud");
        saveButton.setSize(100, 50);
        saveButton.onClick = [this] {makeHttpRequest();};
        addAndMakeVisible (saveButton);
        
        settingName.setSize(100, 50);
        addAndMakeVisible (settingName);
        
        invertButton.setButtonText ("Invert Phase");
        addAndMakeVisible (invertButton);
        invertAttachment.reset (new ButtonAttachment (valueTreeState, "invertPhase", invertButton));

        setSize (400, 200);
    }
    
    void makeHttpRequest ()
    {
        juce::Random random;
        juce:String settingXml = valueTreeState.state.toXmlString();
        adamski::RestRequest request;
        request.header("Content-Type", "application/json");
        request.field("id", String(random.nextInt()));
        request.field("user", "nr");
        request.field("project", settingName.getTextValue().toString());
        request.field("group", "tests");
        request.field("xml", settingXml);
        request.field("settings", "foo");
        request.field("public", true);
        request.field("active", true);
        adamski::RestRequest::Response response = request
        .put ("https://xfmzpgomj5.execute-api.us-west-2.amazonaws.com/dev/settings")
        .execute();
    }

    void resized() override
    {
        auto area = getLocalBounds();

        auto gainRect = area.removeFromTop (paramControlHeight);
        gainLabel .setBounds (gainRect.removeFromLeft (paramLabelWidth));
        gainSlider.setBounds (gainRect);
        saveButton.setBounds (area.removeFromBottom(50));
        settingName.setBounds (area.removeFromBottom(50));

        invertButton.setBounds (area.removeFromTop (paramControlHeight));
    }

    void paint (juce::Graphics& g) override
    {
        g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
    }

private:
    juce::AudioProcessorValueTreeState& valueTreeState;

    juce::Label gainLabel;
    juce::Slider gainSlider;
    juce::TextButton saveButton;
    juce::TextEditor settingName;
    std::unique_ptr<SliderAttachment> gainAttachment;

    juce::ToggleButton invertButton;
    std::unique_ptr<ButtonAttachment> invertAttachment;
};

class XMLSetting {
  public:
    juce::String id;
    juce::String project;
};

class CloudComponent : public juce::Component
{
public:
    CloudComponent (juce::AudioProcessorValueTreeState& vts)
        : valueTreeState (vts)
    {
        addAndMakeVisible (settingsList);
        juce::var cloudSettings = getAllSettings();
        DBG(cloudSettings.size());
        
        
        for (int i = 0; i < cloudSettings.size(); ++i)
            if(cloudSettings[i].getProperty("id", "") != ""){
                settingsList.addItem (cloudSettings[i].getProperty("project", "--").toString(), (int) cloudSettings[i].getProperty("id", 0));
            }
        
        authorizationCode.setSize(100, 50);
        addAndMakeVisible (authorizationCode);
        
        loginButton.setButtonText("Log in");
        loginButton.setSize(100, 50);
        loginButton.onClick = [this] {loginRequest();};
        addAndMakeVisible (loginButton);

        settingsList.setSize(200, 40);
        settingsList.setSelectedId(1);
        
        loadButton.setButtonText("Load Configuration");
        loadButton.setSize(100, 50);
        loadButton.onClick = [this] {stringToXml();};
        addAndMakeVisible (loadButton);

        setSize (200,200);
    }
    
    juce::var getAllSettings ()
    {
        adamski::RestRequest request;
        adamski::RestRequest::Response response = request
        .get ("https://xfmzpgomj5.execute-api.us-west-2.amazonaws.com/dev/settings")
        .execute();
        DBG(response.bodyAsString);
        //juce::Array<XMLSetting> cloudSettings;
        //juce::var parsed = JSON::parse(response.bodyAsString);
        //cloudSettings = (juce::Array<XMLSetting>) parsed;
        return response.body;
    }

    void stringToXml ()
    {
        int selectedSetting = settingsList.getSelectedId();
        DBG(std::to_string(selectedSetting));
        adamski::RestRequest request;
        adamski::RestRequest::Response response = request
        .get ("https://xfmzpgomj5.execute-api.us-west-2.amazonaws.com/dev/settings/object/" + std::to_string(selectedSetting) + "/nr")
        .execute();
        
        DBG(response.bodyAsString);
        
        juce::String xmlResponse = response.body.getProperty("xml", "idx").toString();
        //Array<juce::var> *xmlResponse = response.body.getArray();
        DBG("----------------");
        DBG(response.body.getProperty("xml", "idx").toString());
        DBG("----------------");
    
        //juce::String xmlFromResponse = response.headers.getValue("xml", "");
        
        valueTreeState.replaceState (juce::ValueTree::fromXml (xmlResponse));

    }
    
    void resized() override
    {
        auto area = getLocalBounds();
        authorizationCode.setBounds (area.removeFromTop(50));
        loginButton.setBounds (area.removeFromTop(50));
        settingsList.setBounds (area.removeFromTop(50));
        loadButton.setBounds (area.removeFromTop(50));
    }

    void paint (juce::Graphics& g) override
    {
        g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
    }
    
    void loginRequest ()
    {
        juce::String authCode = authorizationCode.getTextValue().toString();
        adamski::RestRequest request;
        request.header("Content-Type", "application/x-www-form-urlencoded");
        request.header("Authorization", "Basic M2hobDc5bDlrOGMyNTgxNGVwYjhocHVyZm06bTRvZDhnMGlxaGRpZTZwNWpuN29oMHVyNHB2ZG90bDdrYm9sNHJlZDVtdjlxMG9zdjQy");
        adamski::RestRequest::Response response = request
        .post ("https://adc.auth.us-west-2.amazoncognito.com/oauth2/token?grant_type=authorization_code&client_id=3hhl79l9k8c25814epb8hpurfm&code=" + authCode + "&redirect_uri=http://localhost")
        .execute();
        juce::String access_token = response.body.getProperty("access_token", "").toString();
        juce::String userName = userInfoRequest(access_token);
        
        DBG("-------USER NAME---------");
        DBG(userName);
        DBG("--------USER NAME--------");
    }
    
    juce::String userInfoRequest (juce::String access_token)
    {
        adamski::RestRequest request;
        request.header("Content-Type", "application/x-www-form-urlencoded");
        request.header("Authorization", "Bearer " + access_token);
        adamski::RestRequest::Response response = request
        .get ("https://adc.auth.us-west-2.amazoncognito.com/oauth2/userInfo")
        .execute();
        return response.body.getProperty("username", "").toString();
    }

private:
    juce::AudioProcessorValueTreeState& valueTreeState;
    juce::ComboBox settingsList;
    juce::TextButton loadButton;
    juce::TextEditor authorizationCode;
    juce::TextButton loginButton;
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

    GenericEditor (juce::AudioProcessor& parent, juce::AudioProcessorValueTreeState& vts)
        : AudioProcessorEditor (parent),
          valueTreeState (vts)
    {
        addAndMakeVisible(cloud);
        addAndMakeVisible (plugin);
        
        showCloud.setButtonText("Serverless Settings");
        addAndMakeVisible (showCloud);

        setSize (500, 400);
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
    juce::TextButton showCloud;
    CloudComponent* cloud = new CloudComponent(valueTreeState);
    PluginComponent* plugin = new PluginComponent(valueTreeState);
    
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

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override          { return new GenericEditor (*this, parameters); }
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
    }

    void setStateInformation (const void* data, int sizeInBytes) override
    {
        std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));
        if (xmlState.get() != nullptr)
            if (xmlState->hasTagName (parameters.state.getType()))
                parameters.replaceState (juce::ValueTree::fromXml (*xmlState));
    }

private:
    //==============================================================================
    juce::AudioProcessorValueTreeState parameters;
    float previousGain; // [1]

    std::atomic<float>* phaseParameter = nullptr;
    std::atomic<float>* gainParameter  = nullptr;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TutorialProcessor)
};

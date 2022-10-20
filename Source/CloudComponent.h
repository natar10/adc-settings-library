//
//  CloudComponent.h
//  AudioProcessorValueTreeStateTutorial
//
//  Created by Nat Rocha on 14/10/22.
//  Copyright © 2022 JUCE. All rights reserved.
//

#pragma once

#include "Requests.h"
#include <JuceHeader.h>

//==============================================================================

class CloudComponent : public juce::Component, public ValueTree::Listener
{
  public:
    CloudComponent(juce::AudioProcessorValueTreeState& vts, juce::ValueTree& tr, Requests& requests);

    ~CloudComponent();

    void addLoginComponents();
    virtual void valueTreePropertyChanged(ValueTree& tree, const Identifier& property) override;
    void addCloudComponents();
    void updateUserName();
    void updateSettingsList();
    void makeLoginVisible();
    void hideLoginComponents();
    juce::var getAllSettings();
    void stringToXml();
    void checkLogin();
    virtual void resized() override;
    void placeComponentsForLogin();
    void placeComponentsForSettings();
    virtual void paint(juce::Graphics& g) override;
    void loginRequest();
    juce::String userInfoRequest(juce::String access_token);

  private:
    juce::AudioProcessorValueTreeState& valueTreeState;
    juce::ValueTree& tree;
    juce::ComboBox settingsList;
    juce::TextButton loadButton;
    juce::TextEditor authorizationCode;
    juce::TextButton loginButton;
    juce::TextButton refreshButton;
    juce::Label userName;
    juce::TextButton initialLoginButton;
    juce::Label welcome;
    juce::Label results;
    Requests& requestService;
};

//
//  PluginComponent.h
//  AudioProcessorValueTreeStateTutorial
//
//  Created by Nat Rocha on 14/10/22.
//  Copyright © 2022 JUCE. All rights reserved.
//

#pragma once

#include <JuceHeader.h>

//==============================================================================

class PluginComponent : public juce::Component, public ValueTree::Listener
{
  public:
    typedef juce::AudioProcessorValueTreeState::SliderAttachment SliderAttachment;
    typedef juce::AudioProcessorValueTreeState::ButtonAttachment ButtonAttachment;

    PluginComponent(juce::AudioProcessorValueTreeState& vts, juce::ValueTree& tr);

    void toggleSaveToCloud();
    void makeHttpRequest();
    void refreshAndResetForm();
    void toogleSaveButton(bool isEnabled);
    virtual void valueTreePropertyChanged(ValueTree& tree, const Identifier& property) override;
    virtual void resized() override;
    void paint(juce::Graphics& g) override;

  private:
    juce::AudioProcessorValueTreeState& valueTreeState;
    juce::ValueTree& tree;

    juce::Label gainLabel;
    juce::Label saveLabel;
    juce::Slider gainSlider;
    juce::TextButton toggleSave;
    juce::TextButton saveButton;
    juce::TextEditor settingName;
    std::unique_ptr<SliderAttachment> gainAttachment;

    juce::ToggleButton invertButton;
    std::unique_ptr<ButtonAttachment> invertAttachment;

    juce::ToggleButton privateButton;
};

//
//  PluginComponent.h
//  AudioProcessorValueTreeStateTutorial
//
//  Created by Nat Rocha on 14/10/22.
//  Copyright © 2022 JUCE. All rights reserved.
//

#pragma once

//==============================================================================

class PluginComponent : public juce::Component, public ValueTree::Listener
{
  public:
    enum
    {
        paramControlHeight = 40,
        paramLabelWidth = 80,
        paramSliderWidth = 300
    };

    typedef juce::AudioProcessorValueTreeState::SliderAttachment SliderAttachment;
    typedef juce::AudioProcessorValueTreeState::ButtonAttachment ButtonAttachment;

    PluginComponent(juce::AudioProcessorValueTreeState& vts, juce::ValueTree& tr) : valueTreeState(vts), tree(tr)
    {
        gainLabel.setText("Gain", juce::dontSendNotification);
        addAndMakeVisible(gainLabel);

        addAndMakeVisible(gainSlider);
        gainAttachment.reset(new SliderAttachment(valueTreeState, "gain", gainSlider));

        invertButton.setButtonText("Invert Phase");
        addAndMakeVisible(invertButton);
        invertAttachment.reset(new ButtonAttachment(valueTreeState, "invertPhase", invertButton));

        toggleSave.setButtonText("Save to Cloud");
        toggleSave.setSize(100, 30);
        toggleSave.onClick = [this] {
            toggleSaveToCloud();
        };
        addChildComponent(toggleSave);

        DBG("xxxxxxxxTree Changes" << tree.getProperty("isUserActive").toString());
        if (tree.getProperty("isUserActive")) {
            toggleSave.setVisible(true);
        } else {
            toggleSave.setVisible(false);
        }

        saveLabel.setText("Add the name of your setting to save:", juce::dontSendNotification);
        addChildComponent(saveLabel);

        saveButton.setButtonText("Save");
        saveButton.setSize(100, 30);
        saveButton.onClick = [this] {
            makeHttpRequest();
        };
        addChildComponent(saveButton);

        privateButton.setButtonText("This setting should be public");
        addChildComponent(privateButton);

        settingName.setSize(100, 30);
        settingName.setDescription("Add the name of your setting to save:");
        addChildComponent(settingName);

        tree.addListener(this);

        setSize(400, 400);
    }

    void toggleSaveToCloud()
    {
        privateButton.setVisible(!privateButton.isVisible());
        saveLabel.setVisible(!saveLabel.isVisible());
        saveButton.setVisible(!saveButton.isVisible());
        settingName.setVisible(!settingName.isVisible());
        if (saveButton.isVisible()) {
            toggleSave.setButtonText("Hide Save to Cloud");
        } else {
            toggleSave.setButtonText("Save to Cloud");
        }
    }

    void makeHttpRequest()
    {
        juce::Random random;
    juce:
        String settingXml = valueTreeState.state.toXmlString();
        adamski::RestRequest request;
        request.header("Authorization", "Bearer " + tree.getProperty("idToken").toString());
        request.header("Content-Type", "application/json");
        request.field("id", String(random.nextInt()));
        request.field("project", settingName.getTextValue().toString());
        request.field("group", "tests");
        request.field("xml", settingXml);
        request.field("settings", "[]");
        request.field("public", privateButton.getToggleStateValue());
        request.field("active", true);
        adamski::RestRequest::Response response =
            request.put("https://xfmzpgomj5.execute-api.us-west-2.amazonaws.com/dev/settings").execute();
        refreshAndResetForm();
    }

    void refreshAndResetForm()
    {
        settingName.setText("");
        invertButton.setState(juce::ToggleButton::buttonDown);
        toggleSaveToCloud();
    }

    void toogleSaveButton(bool isEnabled)
    {
        toggleSave.setVisible(isEnabled);
    }

    void valueTreePropertyChanged(ValueTree& tree, const Identifier& property) override
    {
        juce::Identifier active = "isUserActive";
        if (property == active) {
            if (tree.hasProperty("isUserActive") && tree.getProperty("isUserActive")) {
                toogleSaveButton(true);
            } else {
                toogleSaveButton(false);
            }
        }
    }

    void resized() override
    {
        auto area = getLocalBounds();

        auto gainRect = area.removeFromTop(paramControlHeight);
        gainLabel.setBounds(gainRect.removeFromLeft(paramLabelWidth));
        gainSlider.setBounds(gainRect);
        invertButton.setBounds(area.removeFromTop(paramControlHeight));

        toggleSave.setBounds(area.removeFromBottom(30));
        saveButton.setBounds(area.removeFromBottom(30));
        settingName.setBounds(area.removeFromBottom(30));
        privateButton.setBounds(area.removeFromBottom(30));
        saveLabel.setBounds(area.removeFromBottom(30));
    }

    void paint(juce::Graphics& g) override
    {
        g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
    }

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

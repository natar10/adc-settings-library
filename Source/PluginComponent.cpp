#include "PluginComponent.h"

enum
{
    paramControlHeight = 40,
    paramLabelWidth = 80,
    paramSliderWidth = 300
};

PluginComponent::PluginComponent(juce::AudioProcessorValueTreeState& vts, juce::ValueTree& tr) :
    valueTreeState(vts), tree(tr)
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

PluginComponent::~PluginComponent()
{
    tree.removeListener(this);
}

void PluginComponent::toggleSaveToCloud()
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

void PluginComponent::makeHttpRequest()
{
    juce::Random random;
    juce::String settingXml = valueTreeState.state.toXmlString();
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

void PluginComponent::refreshAndResetForm()
{
    settingName.setText("");
    invertButton.setState(juce::ToggleButton::buttonDown);
    toggleSaveToCloud();
}

void PluginComponent::toogleSaveButton(bool isEnabled)
{
    toggleSave.setVisible(isEnabled);
}

void PluginComponent::valueTreePropertyChanged(ValueTree& tree, const Identifier& property)
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

void PluginComponent::resized()
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

void PluginComponent::paint(juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

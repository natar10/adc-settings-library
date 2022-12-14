#include "PluginComponent.h"
#include "Types.h"

enum
{
    paramControlHeight = 40,
    paramLabelWidth = 80,
    paramSliderWidth = 300
};

PluginComponent::PluginComponent(juce::AudioProcessorValueTreeState& vts, juce::ValueTree& tr, Requests& requests) :
    valueTreeState(vts), tree(tr), requestService(requests)
{
    gainLabel.setText("Gain", juce::dontSendNotification);
    gainLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    addAndMakeVisible(gainLabel);

    addAndMakeVisible(gainSlider);
    gainAttachment.reset(new SliderAttachment(valueTreeState, "gain", gainSlider));

    invertButton.setButtonText("Invert Phase");
    addAndMakeVisible(invertButton);
    invertAttachment.reset(new ButtonAttachment(valueTreeState, "invertPhase", invertButton));

    toggleSave.setButtonText("Save to Cloud");
    toggleSave.setSize(60, 30);
    toggleSave.setColour(juce::TextButton::buttonColourId, juce::Colours::transparentBlack);
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
    saveLabel.setColour(juce::Label::textColourId, juce::Colours::white);
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
    settingName.setTextToShowWhenEmpty("Setting Name", juce::Colours::grey);
    settingName.setDescription("Add the name of your setting to save:");
    addChildComponent(settingName);
    
    settingDescription.setSize(100, 50);
    settingDescription.setTextToShowWhenEmpty("Setting Description", juce::Colours::grey);
    settingDescription.setDescription("Add a description:");
    addChildComponent(settingDescription);

    tree.addListener(this);

    setSize(Styles::puglinWidth, Styles::defaultHeight);
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
    settingDescription.setVisible(!settingDescription.isVisible());
    if (saveButton.isVisible()) {
        toggleSave.setButtonText("Hide Save to Cloud");
    } else {
        toggleSave.setButtonText("Save to Cloud");
    }
}

void PluginComponent::makeHttpRequest()
{
    SaveSettingsParams saveSettingsParams;
    juce::String settingXml = valueTreeState.state.toXmlString();
    juce::Random random;

    saveSettingsParams.id = abs(random.nextInt());
    saveSettingsParams.project = settingName.getTextValue().toString();
    saveSettingsParams.description = settingDescription.getTextValue().toString();
    saveSettingsParams.group = "vst";
    saveSettingsParams.xml = settingXml;
    saveSettingsParams.settings = "";
    saveSettingsParams.isPublic = static_cast<bool>(privateButton.getToggleStateValue().getValue());
    saveSettingsParams.isActive = true;

    auto response = requestService.saveSettings(IdToken(tree.getProperty("idToken").toString()), saveSettingsParams);

    refreshAndResetForm();
}

void PluginComponent::refreshAndResetForm()
{
    settingName.setText("");
    settingDescription.setText("");
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
    auto area = getLocalBounds().reduced (10.0f);

    auto gainRect = area.removeFromTop(paramControlHeight);
    gainLabel.setBounds(gainRect.removeFromLeft(paramLabelWidth));
    gainSlider.setBounds(gainRect);
    invertButton.setBounds(area.removeFromTop(paramControlHeight));

    toggleSave.setBounds(area.removeFromBottom(30).removeFromRight(70));
    
    saveLabel.setBounds(getWidth()/6.8, 80, getWidth()/1.3, 25);
    privateButton.setBounds(getWidth()/6.6, 105, getWidth()/1.3, 25);
    settingName.setBounds(getWidth()/6, 140, getWidth()/1.5, 25);
    settingDescription.setBounds(getWidth()/6, 175, getWidth()/1.5, 35);
    saveButton.setBounds(getWidth()/6, 220, getWidth()/1.5, 25);
    
}

void PluginComponent::paint(juce::Graphics& g)
{
    g.fillAll (juce::Colour (3, 19, 24));
}

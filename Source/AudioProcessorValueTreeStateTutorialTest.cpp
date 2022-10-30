#include "AudioProcessorValueTreeStateTutorialTest.h"
#include "Requests.h"
#include "XmlData.h"

enum
{
    paramControlHeight = 40,
    paramLabelWidth = 80,
    paramSliderWidth = 300
};

GenericEditor::GenericEditor(juce::AudioProcessor& parent,
                             juce::AudioProcessorValueTreeState& vts,
                             juce::ValueTree& tr,
                             Requests& requests,
                             XmlData& xmlData) :
    AudioProcessorEditor(parent),
    valueTreeState(vts), tree(tr), cloud(new CloudComponent(valueTreeState, tree, requests, xmlData)),
    plugin(new PluginComponent(valueTreeState, tree, requests))
{
    addAndMakeVisible(cloud.get());
    addAndMakeVisible(plugin.get());

    showCloud.setText("SERVERLESS SETTING CLOUD LIBRARY", juce::dontSendNotification);
    showCloud.setColour(juce::Label::backgroundColourId, juce::Colour(18, 18, 19));
    showCloud.setColour(juce::Label::textColourId, juce::Colours::white);
    showCloud.setJustificationType(Justification::centred);
    addAndMakeVisible(showCloud);

    setSize(Styles::editorWidth, Styles::defaultHeight);
}

void GenericEditor::resized()
{
    auto area = getLocalBounds();
    auto headerHeight = area.removeFromTop(36);
    auto fromTop = area.removeFromTop(Styles::editorFromTop);
    showCloud.setBounds(headerHeight);
    cloud->setBounds(fromTop.removeFromLeft(200));
    plugin->setBounds(fromTop);
}

void GenericEditor::paint(juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

TutorialProcessor::TutorialProcessor(Requests& requests, XmlData& xmlData) :
    parameters(*this,
               nullptr,
               juce::Identifier("APVTSTutorial"),
               {
                   std::make_unique<juce::AudioParameterFloat>("gain",        // parameterID
                                                               "Gain",        // parameter name
                                                               0.0f,          // minimum value
                                                               1.0f,          // maximum value
                                                               0.5f),         // default value
                   std::make_unique<juce::AudioParameterBool>("invertPhase",  // parameterID
                                                              "Invert Phase", // parameter name
                                                              false)          // default value
               }),
    requestService(requests),
    xmlDataService(xmlData)
{
    phaseParameter = parameters.getRawParameterValue("invertPhase");
    gainParameter = parameters.getRawParameterValue("gain");
}

void TutorialProcessor::prepareToPlay(double, int)
{
    auto phase = *phaseParameter < 0.5f ? 1.0f : -1.0f;
    previousGain = *gainParameter * phase;
}

void TutorialProcessor::releaseResources()
{
}

void TutorialProcessor::processBlock(juce::AudioSampleBuffer& buffer, juce::MidiBuffer&)
{
    auto phase = *phaseParameter < 0.5f ? 1.0f : -1.0f;
    auto currentGain = *gainParameter * phase;

    if (currentGain == previousGain) {
        buffer.applyGain(currentGain);
    } else {
        buffer.applyGainRamp(0, buffer.getNumSamples(), previousGain, currentGain);
        previousGain = currentGain;
    }
}

juce::AudioProcessorEditor* TutorialProcessor::createEditor()
{
    return new GenericEditor(*this, parameters, tree, requestService, xmlDataService);
}

bool TutorialProcessor::hasEditor() const
{
    return true;
}

const juce::String TutorialProcessor::getName() const
{
    return "APVTS Tutorial";
}

bool TutorialProcessor::acceptsMidi() const
{
    return false;
}

bool TutorialProcessor::producesMidi() const
{
    return false;
}

double TutorialProcessor::getTailLengthSeconds() const
{
    return 0;
}

int TutorialProcessor::getNumPrograms()
{
    return 1;
}

int TutorialProcessor::getCurrentProgram()
{
    return 0;
}

void TutorialProcessor::setCurrentProgram(int)
{
}

const juce::String TutorialProcessor::getProgramName(int)
{
    return {};
}

void TutorialProcessor::changeProgramName(int, const juce::String&)
{
}

void TutorialProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
    if (tree.hasProperty("accessToken"))
        xmlDataService.exportValueTreeToXML(tree.toXmlString());
}

void TutorialProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName(parameters.state.getType()))
            parameters.replaceState(juce::ValueTree::fromXml(*xmlState));
    tree = tree.fromXml(*xmlDataService.loadXMLfromFile());
    // tree.setProperty("isUserActive", userState.isUserLoggedIn, nullptr);
    // tree.setProperty("userName", userState.userName, nullptr);
}

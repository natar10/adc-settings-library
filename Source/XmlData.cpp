#include "XmlData.h"

XmlData::XmlData():
    configFile(File::getSpecialLocation(File::userApplicationDataDirectory).getChildFile("natar10/saveData.xml"))
{
}

void XmlData::exportValueTreeToXML(const String& xmlString)
{
    if (!configFile.exists()) {
        configFile.create();
    }
    
    std::unique_ptr<juce::XmlElement> vtXML(juce::parseXML(xmlString));

    if (!(vtXML->writeTo(configFile))) {
        DBG("FAIL");
    }
}

std::unique_ptr<XmlElement> XmlData::loadXMLfromFile()
{
    if (configFile.existsAsFile()) {
        XmlDocument xmlDoc(configFile);
        return xmlDoc.getDocumentElement();
    }
    return parseXML("<?xml version='1.0' encoding='UTF-8'?><main />");
}

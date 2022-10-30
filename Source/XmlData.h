//
//  XmlData.h
//  AudioProcessorValueTreeStateTutorial
//
//  Created by Nat Rocha on 14/10/22.
//  Copyright © 2022 JUCE. All rights reserved.
//

#pragma once

#include <JuceHeader.h>

//==============================================================================

class XmlData : public juce::Component
{
  public:
    XmlData();

    void exportValueTreeToXML(const String& xmlString);
    std::unique_ptr<XmlElement> loadXMLfromFile();

  private:
    juce::File configFile;
};

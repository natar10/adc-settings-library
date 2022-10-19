//
//  CloudComponent.h
//  AudioProcessorValueTreeStateTutorial
//
//  Created by Nat Rocha on 14/10/22.
//  Copyright © 2022 JUCE. All rights reserved.
//

#pragma once

#include <JuceHeader.h>

//==============================================================================

class Requests : public juce::Component
{
  public:
    Requests();
    
    bool isUserLoggedIn();
    juce::String userInfoRequest(juce::String access_token);
    void loadXMLfromFile();

  private:
    juce::ValueTree requestTree{"main"};
    juce::File desktopDir = File::getSpecialLocation(File::userDesktopDirectory);
    
};

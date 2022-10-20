//
//  CloudComponent.h
//  AudioProcessorValueTreeStateTutorial
//
//  Created by Nat Rocha on 14/10/22.
//  Copyright © 2022 JUCE. All rights reserved.
//

#pragma once

#include <JuceHeader.h>
#include <unordered_map>

#include "Types.h"

//==============================================================================

class Requests : public juce::Component
{
  public:
    Requests();

    struct Endpoints
    {
        static constexpr const char* EXCHANGE_CODE_FOR_TOKEN_ENDPOINT =
            "https://xfmzpgomj5.execute-api.us-west-2.amazonaws.com/dev/auth";

        static constexpr const char* USER_INFO_ENDPOINT =
            "https://adc.auth.us-west-2.amazoncognito.com/oauth2/userInfo";
    };

    LoginState isUserLoggedIn(const AccessToken& accessToken);
    juce::String userInfoRequest(const AccessToken& accessToken);
    void loadXMLfromFile();

  private:
    std::unordered_map<juce::String, LoginState> loginCache;
    juce::File desktopDir = File::getSpecialLocation(File::userDesktopDirectory);
};

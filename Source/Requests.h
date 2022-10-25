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

        static constexpr const char* ALL_SETTINGS_ENDPOINT =
            "https://xfmzpgomj5.execute-api.us-west-2.amazonaws.com/dev/settings";

        static juce::String getSettingEndpoint(int settingName)
        {
            return juce::String(ALL_SETTINGS_ENDPOINT) + "/object/" + std::to_string(settingName);
        }
    };

    LoginState isUserLoggedIn(const AccessToken& accessToken);
    adamski::RestRequest::Response userInfoRequest(const AccessToken& accessToken);
    adamski::RestRequest::Response exchangeAuthorizationCodeForTokens(const AuthorizationCode& authorizationToken);
    adamski::RestRequest::Response getSetting(const IdToken& idToken, int selectedSettingId);
    adamski::RestRequest::Response getAllSettings(const IdToken& idToken);
    adamski::RestRequest::Response saveSettings(const IdToken& idToken, const SaveSettingsParams& saveSettingsRequest);

    void loadXMLfromFile();

  private:
    std::unordered_map<juce::String, LoginState> loginCache;
    juce::File desktopDir = File::getSpecialLocation(File::userDesktopDirectory);
};

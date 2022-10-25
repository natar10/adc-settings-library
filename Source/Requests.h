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

        struct Server
        {
            static constexpr const char* SERVER_URL = "https://xfmzpgomj5.execute-api.us-west-2.amazonaws.com/";

            static constexpr const char* EXCHANGE_CODE_FOR_TOKEN_ENDPOINT = "dev/auth";

            static constexpr const char* ALL_SETTINGS_ENDPOINT = "dev/settings";

            static juce::String getSettingEndpoint(int settingName)
            {
                juce::String endpoint(ALL_SETTINGS_ENDPOINT);
                return endpoint + "/object/" + std::to_string(settingName);
            }
        };

        struct Auth
        {
            static constexpr const char* AUTH_SERVER_URL = "https://adc.auth.us-west-2.amazoncognito.com/";
            static constexpr const char* LOGIN_URL = "https://bit.ly/adclogin";

            static constexpr const char* USER_INFO_ENDPOINT =
                "https://adc.auth.us-west-2.amazoncognito.com/oauth2/userInfo";
        };
    };

    LoginState isUserLoggedIn(const AccessToken& accessToken);
    void openLoginPageInDefaultWebBrowser() const;
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

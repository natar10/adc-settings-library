//
//  Types.h
//  AudioProcessorValueTreeStateTutorial
//
//  Created by Nat Rocha on 19/10/22.
//  Copyright © 2022 JUCE. All rights reserved.
//

#pragma once

//==============================================================================

struct LoginState
{
    bool isUserLoggedIn{false};
    juce::String userName{""};
    juce::String id{""};
};

struct AccessToken
{
    explicit AccessToken(juce::String token) : accessToken(token)
    {
    }

    juce::String toString() const
    {
        return accessToken;
    }

  private:
    juce::String accessToken;
};

struct IdToken
{
    explicit IdToken(juce::String token) : idToken(token)
    {
    }

    juce::String toString() const
    {
        return idToken;
    }

  private:
    juce::String idToken;
};

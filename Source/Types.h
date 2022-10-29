//
//  Types.h
//  AudioProcessorValueTreeStateTutorial
//
//  Created by Nat Rocha on 19/10/22.
//  Copyright © 2022 JUCE. All rights reserved.
//

#pragma once

#include <JuceHeader.h>

//==============================================================================

struct LoginState
{
    bool isUserLoggedIn{false};
    juce::String userName{""};
    juce::String id{""};
};

struct SaveSettingsParams
{
    int id;
    juce::String project;
    juce::String description;
    juce::String group;
    juce::String xml;
    juce::String settings;
    bool isPublic;
    bool isActive;
};

struct AccessToken
{
    explicit AccessToken(const juce::String& token) : accessToken(token)
    {
    }

    juce::String toString() const
    {
        return accessToken;
    }

  private:
    juce::String accessToken;
};

struct AuthorizationCode
{
    explicit AuthorizationCode(const juce::String& token) : authorizationToken(token)
    {
    }

    juce::String toString() const
    {
        return authorizationToken;
    }

  private:
    juce::String authorizationToken;
};

struct IdToken
{
    explicit IdToken(const juce::String& token) : idToken(token)
    {
    }

    juce::String toString() const
    {
        return idToken;
    }

  private:
    juce::String idToken;
};

struct Styles
{
    constexpr static int puglinWidth = 400;
    constexpr static int cloudWidth = 500;
    constexpr static int editorWidth = 500;
    constexpr static int defaultHeight = 340;
    constexpr static int editorFromTop = 300;
};


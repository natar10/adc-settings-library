#include "Requests.h"

Requests::Requests()
{
}

LoginState Requests::isUserLoggedIn(const AccessToken& accessToken)
{
    auto accessTokenString = accessToken.toString();

    if (accessTokenString.isEmpty()) {
        return {};
    }

    if (loginCache.find(accessTokenString) != loginCache.end()) {
        return loginCache[accessTokenString];
    }

    auto response = userInfoRequest(accessToken);
    juce::String userName = "";

    if (response.status == 200) {
        userName = response.body.getProperty("username", "").toString();
    }

    if (userName.isEmpty()) {
        LoginState state{false, ""};
        loginCache[accessTokenString] = state;

        return state;
    }

    LoginState state{true, userName};
    loginCache[accessTokenString] = state;

    return loginCache[accessTokenString];
}

adamski::RestRequest::Response Requests::userInfoRequest(const AccessToken& accessToken)
{
    adamski::RestRequest request(Requests::Endpoints::Auth::AUTH_SERVER_URL);
    request.header("Content-Type", "application/x-www-form-urlencoded");
    request.header("Authorization", "Bearer " + accessToken.toString());

    return request.get(Requests::Endpoints::Auth::USER_INFO_ENDPOINT).execute();
}

adamski::RestRequest::Response Requests::exchangeAuthorizationCodeForTokens(const AuthorizationCode& authorizationToken)
{
    adamski::RestRequest request(Requests::Endpoints::Server::SERVER_URL);
    request.header("Content-Type", "application/json");
    request.field("code", authorizationToken.toString());

    return request.post(Requests::Endpoints::Server::EXCHANGE_CODE_FOR_TOKEN_ENDPOINT).execute();
}

adamski::RestRequest::Response Requests::getSetting(const IdToken& idToken, int selectedSettingId)
{
    adamski::RestRequest request(Requests::Endpoints::Server::SERVER_URL);
    request.header("Authorization", "Bearer " + idToken.toString());

    return request.get(Requests::Endpoints::Server::getSettingEndpoint(selectedSettingId)).execute();
}

adamski::RestRequest::Response Requests::getAllSettings(const IdToken& idToken)
{
    adamski::RestRequest request(Requests::Endpoints::Server::SERVER_URL);
    request.header("Authorization", "Bearer " + idToken.toString());

    return request.get(Requests::Endpoints::Server::ALL_SETTINGS_ENDPOINT).execute();
}

adamski::RestRequest::Response Requests::saveSettings(const IdToken& idToken,
                                                      const SaveSettingsParams& saveSettingsParams)
{
    adamski::RestRequest request(Requests::Endpoints::Server::SERVER_URL);

    request.header("Authorization", "Bearer " + idToken.toString());
    request.header("Content-Type", "application/json");
    request.field("id", juce::String(saveSettingsParams.id));
    request.field("project", saveSettingsParams.project);
    request.field("group", saveSettingsParams.group);
    request.field("xml", saveSettingsParams.xml);
    request.field("settings", saveSettingsParams.settings);
    request.field("public", saveSettingsParams.isPublic);
    request.field("active", saveSettingsParams.isActive);

    return request.put(Requests::Endpoints::Server::ALL_SETTINGS_ENDPOINT).execute();
}

void Requests::openLoginPageInDefaultWebBrowser() const
{
    juce::URL(Requests::Endpoints::Auth::LOGIN_URL).launchInDefaultBrowser();
}

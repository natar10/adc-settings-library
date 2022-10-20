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

    auto userInfo = userInfoRequest(accessToken);

    if (userInfo == "Request Error") {
        LoginState state{};
        loginCache[accessTokenString] = state;
        return {false, ""};
    } else {
        LoginState state{true, userInfo};
        loginCache[accessTokenString] = state;
    }

    return loginCache[accessTokenString];
}

juce::String Requests::userInfoRequest(const AccessToken& accessToken)
{
    adamski::RestRequest request;
    request.header("Content-Type", "application/x-www-form-urlencoded");
    request.header("Authorization", "Bearer " + accessToken.toString());
    adamski::RestRequest::Response response =
        request.get("https://adc.auth.us-west-2.amazoncognito.com/oauth2/userInfo").execute();

    if (response.status == 200) {
        return response.body.getProperty("username", "").toString();
    } else {
        return "Request Error";
    }
}

void Requests::loadXMLfromFile()
{
    // File readFrom(desktopDir.getChildFile("saveData.xml"));
    // if (readFrom.existsAsFile()) {
    // XmlDocument xmlDoc(readFrom);
    // if (auto mainElement = xmlDoc.getDocumentElement())
    // requestTree = requestTree.fromXml(*mainElement);
    // }
}

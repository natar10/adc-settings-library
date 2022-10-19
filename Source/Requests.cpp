#include "Requests.h"

Requests::Requests()
{
}

bool Requests::isUserLoggedIn()
{
    if (requestTree.hasProperty("accessToken")) {
        String userInfo = userInfoRequest(requestTree["accessToken"]);
        if (userInfo == "Request Error") {
            return false;
        } else {
            return true;
        }
    } else {
        return false;
    }
}

juce::String Requests::userInfoRequest(juce::String access_token)
{
    adamski::RestRequest request;
    request.header("Content-Type", "application/x-www-form-urlencoded");
    request.header("Authorization", "Bearer " + access_token);
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
    File readFrom(desktopDir.getChildFile("saveData.xml"));
    if (readFrom.existsAsFile()) {
        XmlDocument xmlDoc(readFrom);
        if (auto mainElement = xmlDoc.getDocumentElement())
            requestTree = requestTree.fromXml(*mainElement);
    }
}

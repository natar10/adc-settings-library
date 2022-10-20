#include "CloudComponent.h"

CloudComponent::CloudComponent(juce::AudioProcessorValueTreeState& vts, juce::ValueTree& tr, Requests& requests) :
    valueTreeState(vts), tree(tr), requestService(requests)
{
    addAndMakeVisible(welcome);
    checkLogin();
    setSize(500, 500);
    tree.addListener(this);
}

CloudComponent::~CloudComponent()
{
    tree.removeListener(this);
}

void CloudComponent::addLoginComponents()
{
    welcome.setText("Login here to sync your settings in the cloud", juce::dontSendNotification);

    initialLoginButton.setButtonText("Log in");
    initialLoginButton.setSize(100, 30);
    initialLoginButton.onClick = [this] {
        makeLoginVisible();
    };
    addAndMakeVisible(initialLoginButton);
    addAndMakeVisible(userName);

    authorizationCode.setSize(100, 30);
    addChildComponent(authorizationCode);

    loginButton.setButtonText("Log in");
    loginButton.setSize(100, 30);
    loginButton.onClick = [this] {
        loginRequest();
    };
    addChildComponent(loginButton);
}

void CloudComponent::valueTreePropertyChanged(ValueTree& tree, const Identifier& property)
{

    DBG("************LISTENER LISTENER LISTENER ***************");
    DBG("**PROPERTY**: " << property);
    DBG("**VALUE**: " << tree.toXmlString());
    juce::Identifier active = "isUserActive";
    if (property == active) {
        if (tree.hasProperty("accessToken")) {
            updateUserName();
        }
        DBG("Tree Changes" << tree.getProperty("isUserActive").toString());
        if (tree.hasProperty("isUserActive") && tree.getProperty("isUserActive")) {
            DBG("Tree isUserActive exists and is true");

            DBG("************LISTENER LLAMA A CLOUD****************");
            addCloudComponents();
            hideLoginComponents();
            placeComponentsForSettings();
            // plugin->toogleSaveButton(true);
        } else {
            DBG("Tree isUserActive is false");
            addLoginComponents();
            placeComponentsForLogin();
            // plugin->toogleSaveButton(false);
        }
    }
}

void CloudComponent::addCloudComponents()
{
    DBG("ENTRA A AD CLOUD COMP");

    addChildComponent(settingsList);
    addChildComponent(results);
    addChildComponent(loadButton);
    addChildComponent(refreshButton);
    addChildComponent(userName);

    updateSettingsList();

    loadButton.setButtonText("Load Configuration");
    loadButton.setSize(100, 30);
    loadButton.onClick = [this] {
        stringToXml();
    };

    refreshButton.setButtonText("Refresh");
    refreshButton.setSize(100, 30);
    refreshButton.onClick = [this] {
        updateSettingsList();
    };

    DBG("finaliza A AD CLOUD COMP");
}

void CloudComponent::updateUserName()
{
    userName.setText(userInfoRequest(tree["accessToken"]), juce::dontSendNotification);
}

void CloudComponent::updateSettingsList()
{
    refreshButton.setButtonText("Loading...");
    refreshButton.setEnabled(false);
    juce::var cloudSettings = getAllSettings();

    settingsList.clear();

    for (int i = 0; i < cloudSettings.size(); ++i)
        if (cloudSettings[i].getProperty("id", "") != "") {
            settingsList.addItem(cloudSettings[i].getProperty("project", "--").toString(),
                                 (int)cloudSettings[i].getProperty("id", 0));
        }

    settingsList.setSize(200, 30);
    settingsList.setSelectedId(1);
    refreshButton.setButtonText("Refresh");
    refreshButton.setEnabled(true);
}

void CloudComponent::makeLoginVisible()
{
    system("open https://bit.ly/adclogin");
    initialLoginButton.setVisible(false);
    welcome.setText("Login in the browser and paste your authorization code here:", juce::dontSendNotification);
    loginButton.setVisible(true);
    authorizationCode.setVisible(true);
}

void CloudComponent::hideLoginComponents()
{
    DBG("ENTRA HIDE LOGIN COMP");
    initialLoginButton.setVisible(false);
    loginButton.setVisible(false);
    authorizationCode.setVisible(false);
    userName.setVisible(true);
    settingsList.setVisible(true);
    loadButton.setVisible(true);
    refreshButton.setVisible(true);
    results.setVisible(true);
    welcome.setText("Welcome, this are your cloud available settings:", juce::dontSendNotification);

    DBG("finaliza HIDE LOGIN COMP");
}

juce::var CloudComponent::getAllSettings()
{
    DBG("-----------GET ALL SETTINGS----------");
    adamski::RestRequest request;
    request.header("Authorization", "Bearer " + tree.getProperty("idToken").toString());
    adamski::RestRequest::Response response = request
                                                  .get("https://xfmzpgomj5.execute-api.us-west-2.amazonaws.com/dev/"
                                                       "settings")
                                                  .execute();
    return response.body;
}

void CloudComponent::stringToXml()
{
    int selectedSetting = settingsList.getSelectedId();
    adamski::RestRequest request;
    request.header("Authorization", "Bearer " + tree.getProperty("idToken").toString());
    adamski::RestRequest::Response response = request
                                                  .get("https://xfmzpgomj5.execute-api.us-west-2.amazonaws.com/dev/"
                                                       "settings/object/" +
                                                       std::to_string(selectedSetting))
                                                  .execute();
    if (response.status != 200) {
        results.setText("There was an error loading this setting", juce::dontSendNotification);
    } else {
        juce::String xmlResponse = response.body.getProperty("xml", "idx").toString();
        valueTreeState.replaceState(juce::ValueTree::fromXml(xmlResponse));
        results.setText("Setting loaded", juce::dontSendNotification);
    }
}

void CloudComponent::checkLogin()
{
    DBG("Tree CHECK LOGIN");
    bool isUserActive = tree.getProperty("isUserActive");
    DBG("*****isUserActive****" << tree.getProperty("isUserActive").toString());

    if (!isUserActive) {
        addLoginComponents();
        userName.setText("Please Login", juce::dontSendNotification);
        tree.setProperty("isUserActive", false, nullptr);
    } else {
        userName.setText(tree.getProperty("userName"), juce::dontSendNotification);
        DBG("************LOGGED IN****************");
        addCloudComponents();
        hideLoginComponents();
        tree.setProperty("isUserActive", true, nullptr);
    }

    DBG("Tree ISUSER--" << tree.getProperty("isUserActive").toString());
}

void CloudComponent::resized()
{
    if (tree.getProperty("isUserActive")) {
        placeComponentsForSettings();
    } else {
        placeComponentsForLogin();
    }
}

void CloudComponent::placeComponentsForLogin()
{
    auto area = getLocalBounds();
    welcome.setBounds(area.removeFromTop(50));
    initialLoginButton.setBounds(area.removeFromTop(30));
    authorizationCode.setBounds(area.removeFromTop(30));
    loginButton.setBounds(area.removeFromTop(30));
    userName.setBounds(area.removeFromTop(30));
    refreshButton.setBounds(area.removeFromTop(30));
    settingsList.setBounds(area.removeFromTop(30));
    loadButton.setBounds(area.removeFromTop(30));
    results.setBounds(area.removeFromTop(30));
}

void CloudComponent::placeComponentsForSettings()
{
    auto area = getLocalBounds();
    welcome.setBounds(area.removeFromTop(50));
    userName.setBounds(area.removeFromTop(30));
    refreshButton.setBounds(area.removeFromTop(30));
    settingsList.setBounds(area.removeFromTop(30));
    loadButton.setBounds(area.removeFromTop(30));
    results.setBounds(area.removeFromTop(30));
    initialLoginButton.setBounds(area.removeFromTop(30));
    authorizationCode.setBounds(area.removeFromTop(30));
    loginButton.setBounds(area.removeFromTop(30));
}

void CloudComponent::paint(juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void CloudComponent::loginRequest()
{
    loginButton.setButtonText("Loading...");
    loginButton.setEnabled(false);
    juce::String authCode = authorizationCode.getTextValue().toString();
    adamski::RestRequest request;
    request.header("Content-Type", "application/json");
    request.field("code", authCode);
    adamski::RestRequest::Response response = request
                                                  .post("https://xfmzpgomj5.execute-api.us-west-2.amazonaws.com/dev/"
                                                        "auth")
                                                  .execute();
    juce::String access_token = response.body.getProperty("access_token", "").toString();
    juce::String id_token = response.body.getProperty("id_token", "").toString();
    juce::String refresh_token = response.body.getProperty("refresh_token", "").toString();

    if (response.status != 200) {
        tree.setProperty("isUserActive", false, nullptr);
        welcome.setText("There was an error with the login, please try again.", juce::dontSendNotification);
        loginButton.setEnabled(true);
        loginButton.setButtonText("Login");
    } else {
        hideLoginComponents();
        tree.setPropertyExcludingListener(this, "accessToken", access_token, nullptr);
        tree.setPropertyExcludingListener(this, "idToken", id_token, nullptr);
        tree.setPropertyExcludingListener(this, "refreshToken", refresh_token, nullptr);
        tree.setProperty("isUserActive", true, nullptr);
    }
}

juce::String CloudComponent::userInfoRequest(juce::String access_token)
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

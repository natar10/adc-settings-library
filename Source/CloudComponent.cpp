#include "CloudComponent.h"

CloudComponent::CloudComponent(juce::AudioProcessorValueTreeState& vts, juce::ValueTree& tr, Requests& requests) :
    valueTreeState(vts), tree(tr), requestService(requests)
{
    getLookAndFeel().setColour (juce::Label::textColourId, juce::Colours::darkgrey);
    addAndMakeVisible(welcome);
    addAndMakeVisible(title);
    checkLogin();
    setSize(500, 300);
    tree.addListener(this);
}

CloudComponent::~CloudComponent()
{
    tree.removeListener(this);
}

void CloudComponent::addLoginComponents()
{
    title.setText("INTO THE CLOUD", juce::dontSendNotification);
    title.setColour(juce::Label::textColourId, juce::Colours::darkslategrey);
    title.setJustificationType(Justification::horizontallyCentred);
    
    welcome.setText("Login and sync up settings", juce::dontSendNotification);
    welcome.setJustificationType(Justification::horizontallyJustified);
    
    initialLoginButton.setButtonText("Start Here");
    initialLoginButton.setSize(100, 30);
    initialLoginButton.onClick = [this] {
        makeLoginVisible();
    };
    addAndMakeVisible(initialLoginButton);
    addChildComponent(userName);

    authorizationCode.setSize(100, 30);
    authorizationCode.setTextToShowWhenEmpty("xxx-xxx-xxx", juce::Colours::grey);
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
    refreshButton.setColour(juce::TextButton::buttonColourId, juce::Colours::white);
    refreshButton.setColour(juce::TextButton::textColourOffId, juce::Colours::darkgrey);
    refreshButton.setSize(100, 30);
    refreshButton.onClick = [this] {
        updateSettingsList();
    };
}

void CloudComponent::updateUserName()
{
    userName.setText(userInfoRequest(tree["accessToken"]), juce::dontSendNotification);
}

void CloudComponent::updateSettingsList()
{
    refreshButton.setEnabled(false);
    juce::var cloudSettings = getAllSettings();

    settingsList.clear();

    for (int i = 0; i < cloudSettings.size(); ++i)
        if (cloudSettings[i].getProperty("id", "") != "") {
            settingsList.addItem(cloudSettings[i].getProperty("project", "--").toString(),
                                 (int)cloudSettings[i].getProperty("id", 0));
        }

    settingsList.setSelectedId(1);
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
    initialLoginButton.setVisible(false);
    loginButton.setVisible(false);
    authorizationCode.setVisible(false);
    userName.setVisible(true);
    settingsList.setVisible(true);
    loadButton.setVisible(true);
    refreshButton.setVisible(true);
    results.setVisible(true);
    welcome.setText("Welcome! These are your cloud settings:", juce::dontSendNotification);
    welcome.setJustificationType(Justification::centred);
}

juce::var CloudComponent::getAllSettings()
{
    return requestService.getAllSettings(IdToken(tree.getProperty("idToken").toString())).body;
}

void CloudComponent::stringToXml()
{
    int selectedSetting = settingsList.getSelectedId();
    auto response = requestService.getSetting(IdToken(tree.getProperty("idToken").toString()), selectedSetting);

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
    bool isUserActive = tree.getProperty("isUserActive");
 
    if (!isUserActive) {
        addLoginComponents();
        userName.setText("Please Login", juce::dontSendNotification);
        tree.setProperty("isUserActive", false, nullptr);
    } else {
        userName.setText(tree.getProperty("userName"), juce::dontSendNotification);
        addCloudComponents();
        hideLoginComponents();
        tree.setProperty("isUserActive", true, nullptr);
    }
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
    title.setBounds(0, 20, getWidth(), 20);
    welcome.setBounds(getX()+12, 50, getWidth()-24, 50);
    initialLoginButton.setBounds(getWidth()/4, 110, getWidth()/2, 30);
    authorizationCode.setBounds((getWidth()-(getWidth()/1.4))/2, 110, getWidth()/1.4, 30);
    loginButton.setBounds(getWidth()/4, 150, getWidth()/2, 30);
}

void CloudComponent::placeComponentsForSettings()
{
    welcome.setBounds(getX()+12, 25, getWidth()-24, 70);
    userName.setBounds(getX()+12, 50, getWidth(), 30);
    refreshButton.setBounds(getWidth()-80, 85, 60, 20);
    settingsList.setBounds(getX()+20, 110, getWidth()-40, 25);
    loadButton.setBounds(getWidth()/6, 145, getWidth()/1.5, 25);
    results.setBounds(getX()+12, 175, getWidth()-12, 30);
}

void CloudComponent::paint(juce::Graphics& g)
{
    g.fillAll (juce::Colours::white);
    g.setColour (juce::Colours::grey);
    auto centralArea = getLocalBounds().toFloat().reduced (10.0f);
    g.drawRoundedRectangle (centralArea, 5.0f, 3.0f);
}

void CloudComponent::loginRequest()
{
    loginButton.setEnabled(false);
    juce::String authCode = authorizationCode.getTextValue().toString();

    auto response = requestService.exchangeAuthorizationCodeForTokens(AuthorizationCode(authCode));

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

juce::String CloudComponent::userInfoRequest(juce::String accessToken)
{
    auto response = requestService.userInfoRequest(AccessToken(accessToken));

    if (response.status == 200) {
        return response.body.getProperty("username", "").toString();
    } else {
        return "Request Error";
    }
}

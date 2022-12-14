#include "CloudComponent.h"

CloudComponent::CloudComponent(juce::AudioProcessorValueTreeState& vts, juce::ValueTree& tr, Requests& requests, XmlData& xmlData) :
    valueTreeState(vts), tree(tr), requestService(requests), xmlDataService(xmlData)
{
    getLookAndFeel().setColour (juce::Label::textColourId, juce::Colours::darkgrey);
    addAndMakeVisible(welcome);
    addAndMakeVisible(title);
    checkLogin();
    setSize(Styles::cloudWidth, Styles::defaultHeight);
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
        if (tree.hasProperty("isUserActive") && tree.getProperty("isUserActive")) {
            addCloudComponents();
            hideLoginComponents();
            placeComponentsForSettings();
        } else {
            addLoginComponents();
            hideCloudComponents();
            placeComponentsForLogin();
        }
    }
}

void CloudComponent::addCloudComponents()
{
    addChildComponent(settingsList);
    addChildComponent(results);
    addChildComponent(loadButton);
    addChildComponent(logout);
    addChildComponent(refreshButton);
    addChildComponent(userName);

    settingsList.onChange = [this] {
        enableLoadButton();
    };
    updateSettingsList();

    loadButton.setButtonText("Load Configuration");
    loadButton.setEnabled(false);
    loadButton.setSize(100, 30);
    loadButton.onClick = [this] {
        stringToXml();
    };
    
    logout.setButtonText("Logout");
    logout.setSize(60, 30);
    logout.setColour(juce::TextButton::buttonColourId, juce::Colours::transparentBlack);
    logout.setColour(juce::TextButton::textColourOffId, juce::Colours::black);
    logout.setColour(juce::TextButton::textColourOnId, juce::Colours::black);
    logout.onClick = [this] { logoutRequest(); };

    refreshButton.setButtonText("Refresh");
    refreshButton.setColour(juce::TextButton::buttonColourId, juce::Colours::white);
    refreshButton.setColour(juce::TextButton::textColourOffId, juce::Colours::darkgrey);
    refreshButton.setSize(100, 30);
    refreshButton.onClick = [this] {
        updateSettingsList();
    };
}

void CloudComponent::enableLoadButton()
{
    loadButton.setEnabled(settingsList.getSelectedId());
}

void CloudComponent::hideCloudComponents()
{
    settingsList.setVisible(false);
    results.setVisible(false);
    loadButton.setVisible(false);
    logout.setVisible(false);
    refreshButton.setVisible(false);
    userName.setVisible(false);
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
    requestService.openLoginPageInDefaultWebBrowser();

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
    logout.setVisible(true);
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
    LoginState loginState =  requestService.isUserLoggedIn(AccessToken(tree.getProperty("accessToken")));
    
    if (!loginState.isUserLoggedIn) {
        DBG("************NOT LOGGED IN****************");
        addLoginComponents();
        userName.setText("Please Login", juce::dontSendNotification);
        tree.setProperty("isUserActive", false, nullptr);
    } else {
        userName.setText(loginState.userName, juce::dontSendNotification);
        DBG("************LOGGED IN****************");
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
    auto area = getLocalBounds().reduced (14.0f);
    welcome.setBounds(getX()+12, 25, getWidth()-24, 70);
    userName.setBounds(getX()+12, 75, getWidth(), 30);
    refreshButton.setBounds(getWidth()-80, 105, 60, 20);
    settingsList.setBounds(getX()+20, 130, getWidth()-40, 25);
    loadButton.setBounds(getWidth()/6, 165, getWidth()/1.5, 25);
    results.setBounds(getX()+12, 195, getWidth()-12, 30);
    logout.setBounds(area.removeFromBottom(25).removeFromRight(70));
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
        tree.setPropertyExcludingListener(this, "accessToken", access_token, nullptr);
        tree.setPropertyExcludingListener(this, "idToken", id_token, nullptr);
        tree.setPropertyExcludingListener(this, "refreshToken", refresh_token, nullptr);
        tree.setProperty("isUserActive", true, nullptr);
        xmlDataService.exportValueTreeToXML(tree.toXmlString());
    }
}

void CloudComponent::logoutRequest()
{
    logout.setEnabled(false);
    tree.setProperty("isUserActive", false, nullptr);
    tree.removeAllProperties(nullptr);
    xmlDataService.exportValueTreeToXML(tree.toXmlString());
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

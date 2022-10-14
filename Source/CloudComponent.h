//
//  CloudComponent.h
//  AudioProcessorValueTreeStateTutorial
//
//  Created by Nat Rocha on 14/10/22.
//  Copyright © 2022 JUCE. All rights reserved.
//

#ifndef CloudComponent_h
#define CloudComponent_h


#endif /* CloudComponent_h */

#pragma once

//==============================================================================

class CloudComponent : public juce::Component, public ValueTree::Listener
{
public:
    CloudComponent (juce::AudioProcessorValueTreeState& vts, juce::ValueTree& tr)
        : valueTreeState (vts), tree(tr)
    {
        addAndMakeVisible (welcome);
        checkLogin();
        setSize(500, 500);
        tree.addListener(this);
    }
    
    void addLoginComponents ()
    {
        welcome.setText("Login here to sync your settings in the cloud", juce::dontSendNotification);
        
        initialLoginButton.setButtonText("Log in");
        initialLoginButton.setSize(100, 30);
        initialLoginButton.onClick = [this] {makeLoginVisible();};
        addAndMakeVisible (initialLoginButton);
        addAndMakeVisible (userName);

        authorizationCode.setSize(100, 30);
        addChildComponent (authorizationCode);
        
        loginButton.setButtonText("Log in");
        loginButton.setSize(100, 30);
        loginButton.onClick = [this] {loginRequest();};
        addChildComponent (loginButton);
    }
    
    void valueTreePropertyChanged (ValueTree &tree, const Identifier &property) override
    {
        
        DBG("************LISTENER LISTENER LISTENER ***************");
        DBG("**PROPERTY**: " << property);
        DBG("**VALUE**: " << tree.toXmlString());
        juce::Identifier active = "isUserActive";
        if(property == active){
            if(tree.hasProperty("accessToken")){
                updateUserName();
            }
            DBG("Tree Changes" << tree.getProperty("isUserActive").toString());
            if(tree.hasProperty("isUserActive") && tree.getProperty("isUserActive")){
                DBG("Tree isUserActive exists and is true");
                
                DBG("************LISTENER LLAMA A CLOUD****************");
                addCloudComponents();
                hideLoginComponents();
                placeComponentsForSettings();
                //plugin->toogleSaveButton(true);
            }else{
                DBG("Tree isUserActive is false");
                addLoginComponents();
                placeComponentsForLogin();
                //plugin->toogleSaveButton(false);
            }
        }
    }
    
    void addCloudComponents (){
        DBG("ENTRA A AD CLOUD COMP");
        
        addChildComponent (settingsList);
        addChildComponent (results);
        addChildComponent (loadButton);
        addChildComponent (refreshButton);
        addChildComponent (userName);
        
        updateSettingsList();
        
        loadButton.setButtonText("Load Configuration");
        loadButton.setSize(100, 30);
        loadButton.onClick = [this] {stringToXml();};
        
        refreshButton.setButtonText("Refresh");
        refreshButton.setSize(100, 30);
        refreshButton.onClick = [this] {updateSettingsList();};
        
        DBG("finaliza A AD CLOUD COMP");
    }
    
    void updateUserName(){
        userName.setText(userInfoRequest(tree["accessToken"]), juce::dontSendNotification);
    }
    
    void updateSettingsList (){
        refreshButton.setButtonText("Loading...");
        refreshButton.setEnabled(false);
        juce::var cloudSettings = getAllSettings();
        
        settingsList.clear();

        for (int i = 0; i < cloudSettings.size(); ++i)
            if(cloudSettings[i].getProperty("id", "") != ""){
                settingsList.addItem (cloudSettings[i].getProperty("project", "--").toString(), (int) cloudSettings[i].getProperty("id", 0));
            }
        
        settingsList.setSize(200, 30);
        settingsList.setSelectedId(1);
        refreshButton.setButtonText("Refresh");
        refreshButton.setEnabled(true);
    }
    
    void makeLoginVisible ()
    {
        system("open https://bit.ly/adclogin");
        initialLoginButton.setVisible(false);
        welcome.setText("Login in the browser and paste your authorization code here:", juce::dontSendNotification);
        loginButton.setVisible(true);
        authorizationCode.setVisible(true);
    }
    
    void hideLoginComponents ()
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
    
    juce::var getAllSettings ()
    {
        DBG("-----------GET ALL SETTINGS----------");
        adamski::RestRequest request;
        request.header("Authorization", "Bearer " + tree.getProperty("idToken").toString());
        adamski::RestRequest::Response response = request
        .get ("https://xfmzpgomj5.execute-api.us-west-2.amazonaws.com/dev/settings")
        .execute();
        return response.body;
    }

    void stringToXml ()
    {
        int selectedSetting = settingsList.getSelectedId();
        adamski::RestRequest request;
        request.header("Authorization", "Bearer " + tree.getProperty("idToken").toString());
        adamski::RestRequest::Response response = request
        .get ("https://xfmzpgomj5.execute-api.us-west-2.amazonaws.com/dev/settings/object/" + std::to_string(selectedSetting))
        .execute();
        if(response.status != 200){
            results.setText("There was an error loading this setting", juce::dontSendNotification);
        }else{
            juce::String xmlResponse = response.body.getProperty("xml", "idx").toString();
            valueTreeState.replaceState (juce::ValueTree::fromXml (xmlResponse));
            results.setText("Setting loaded", juce::dontSendNotification);
        }
        
    }
    
    void checkLogin ()
    {
        DBG("Tree CHECK LOGIN");
        tree.setPropertyExcludingListener(this, "isUserActive", false, nullptr);
        DBG("Tree ISUSER" << tree.getProperty("isUserActive").toString());

        if(tree.hasProperty("accessToken")){
            
            String userInfo = userInfoRequest(tree["accessToken"]);
            DBG("************USER INFO****************" << userInfo);
            if(userInfo == "Request Error"){
                addLoginComponents();
                userName.setText("Please Login", juce::dontSendNotification);
            }else{
                userName.setText(userInfoRequest(tree["accessToken"]), juce::dontSendNotification);
                DBG("************LOGGED IN****************");
                addCloudComponents();
                hideLoginComponents();
                tree.setProperty("isUserActive", true, nullptr);
            }
        }else{
            addLoginComponents();
            userName.setText("You are not logged in", juce::dontSendNotification);
        }
        
        DBG("Tree ISUSER--" << tree.getProperty("isUserActive").toString());
    }
    
    void resized() override
    {
        if(tree.getProperty("isUserActive")){
            placeComponentsForSettings();
        }else{
            placeComponentsForLogin();
        }
    }
    
    void placeComponentsForLogin ()
    {
        auto area = getLocalBounds();
        welcome.setBounds (area.removeFromTop(50));
        initialLoginButton.setBounds (area.removeFromTop(30));
        authorizationCode.setBounds (area.removeFromTop(30));
        loginButton.setBounds (area.removeFromTop(30));
        userName.setBounds (area.removeFromTop(30));
        refreshButton.setBounds (area.removeFromTop(30));
        settingsList.setBounds (area.removeFromTop(30));
        loadButton.setBounds (area.removeFromTop(30));
        results.setBounds (area.removeFromTop(30));
    }
    
    void placeComponentsForSettings ()
    {
        auto area = getLocalBounds();
        welcome.setBounds (area.removeFromTop(50));
        userName.setBounds (area.removeFromTop(30));
        refreshButton.setBounds (area.removeFromTop(30));
        settingsList.setBounds (area.removeFromTop(30));
        loadButton.setBounds (area.removeFromTop(30));
        results.setBounds (area.removeFromTop(30));
        initialLoginButton.setBounds (area.removeFromTop(30));
        authorizationCode.setBounds (area.removeFromTop(30));
        loginButton.setBounds (area.removeFromTop(30));
    }

    void paint (juce::Graphics& g) override
    {
        g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
    }
    
    void loginRequest ()
    {
        loginButton.setButtonText("Loading...");
        loginButton.setEnabled(false);
        juce::String authCode = authorizationCode.getTextValue().toString();
        adamski::RestRequest request;
        request.header("Content-Type", "application/json");
        request.field("code", authCode);
        adamski::RestRequest::Response response = request
        .post ("https://xfmzpgomj5.execute-api.us-west-2.amazonaws.com/dev/auth")
        .execute();
        juce::String access_token = response.body.getProperty("access_token", "").toString();
        juce::String id_token = response.body.getProperty("id_token", "").toString();
        juce::String refresh_token = response.body.getProperty("refresh_token", "").toString();
        
        if(response.status != 200){
            tree.setProperty("isUserActive", false, nullptr);
            welcome.setText("There was an error with the login, please try again.", juce::dontSendNotification);
            loginButton.setEnabled(true);
            loginButton.setButtonText("Login");
        }else{
            hideLoginComponents();
            tree.setPropertyExcludingListener (this, "accessToken", access_token, nullptr);
            tree.setPropertyExcludingListener (this, "idToken", id_token, nullptr);
            tree.setPropertyExcludingListener(this, "refreshToken", refresh_token, nullptr);
            tree.setProperty ("isUserActive", true, nullptr);
        }
    }
    
    juce::String userInfoRequest (juce::String access_token)
    {
        adamski::RestRequest request;
        request.header("Content-Type", "application/x-www-form-urlencoded");
        request.header("Authorization", "Bearer " + access_token);
        adamski::RestRequest::Response response = request
        .get ("https://adc.auth.us-west-2.amazoncognito.com/oauth2/userInfo")
        .execute();
        
        if(response.status == 200){
            return response.body.getProperty("username", "").toString();
        }else{
            return "Request Error";
        }
    }

private:
    juce::AudioProcessorValueTreeState& valueTreeState;
    juce::ValueTree& tree;
    juce::ComboBox settingsList;
    juce::TextButton loadButton;
    juce::TextEditor authorizationCode;
    juce::TextButton loginButton;
    juce::TextButton refreshButton;
    juce::Label userName;
    juce::TextButton initialLoginButton;
    juce::Label welcome;
    juce::Label results;
};

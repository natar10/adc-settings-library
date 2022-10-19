/*
  ==============================================================================

    This file was auto-generated and contains the startup code for a PIP.

  ==============================================================================
*/

#include <JuceHeader.h>
#include "AudioProcessorValueTreeStateTutorialTest.h"
#include "Requests.h"
#include "Types.h"
int a = 0;
//Create the class here

//==============================================================================
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    DBG("main method called");
    LoginState loginState = {};
    if(a == 0){
        DBG("verify only run once");
        a = 1;
        auto Req = new Requests();
        loginState = Req->isUserLoggedIn();
    }
    return new TutorialProcessor(loginState); //Here pass the reference for this class
}

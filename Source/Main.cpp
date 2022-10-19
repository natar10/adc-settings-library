/*
  ==============================================================================

    This file was auto-generated and contains the startup code for a PIP.

  ==============================================================================
*/

#include <JuceHeader.h>
#include "AudioProcessorValueTreeStateTutorialTest.h"
#include "Requests.h"
int a = 0;
//Create the class here

//==============================================================================
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    DBG("main method called");
    bool isUserLoggedIn = false;
    if(a == 0){
        DBG("verify only run once");
        a = 1;
        auto Req = new Requests();
        isUserLoggedIn = Req->isUserLoggedIn();
    }
    return new TutorialProcessor(isUserLoggedIn); //Here pass the reference for this class
}

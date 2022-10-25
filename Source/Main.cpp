/*
  ==============================================================================

    This file was auto-generated and contains the startup code for a PIP.

  ==============================================================================
*/

#include "AudioProcessorValueTreeStateTutorialTest.h"
#include <JuceHeader.h>
#include "Requests.h"

Requests requestService;

//==============================================================================
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new TutorialProcessor(requestService);
}

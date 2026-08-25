#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include "../Core/JADContext.h"
#include "SessionOverviewPanel.h"
#include "SynthVoicePanel.h"

class MainComponent : public juce::Component
{
public:
    explicit MainComponent (
        JADContext& contextToUse
    );

    void paint (
        juce::Graphics& g
    ) override;

    void resized() override;

private:
    JADContext& context;

    SessionOverviewPanel sessionOverviewPanel;
    SynthVoicePanel synthVoicePanel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (
        MainComponent
    )
};

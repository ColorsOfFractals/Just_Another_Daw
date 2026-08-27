#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include "../Model/SessionState.h"

class SessionOverviewPanel :
    public juce::Component
{
public:
    explicit SessionOverviewPanel (
        SessionState& sessionState
    );

    void paint (
        juce::Graphics& g
    ) override;

private:
    SessionState& session;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (
        SessionOverviewPanel
    )
};

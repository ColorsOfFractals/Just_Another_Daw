#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include "../Core/JADContext.h"

#include "JADLookAndFeel.h"
#include "SessionOverviewPanel.h"
#include "SynthVoicePanel.h"
#include "TransportPanel.h"
#include "VirtualKeyboardPanel.h"

class MainComponent :
    public juce::Component
{
public:
    explicit MainComponent (
        JADContext& contextToUse
    );

    ~MainComponent() override;

    void paint (
        juce::Graphics&
    ) override;

    void resized() override;

private:
    void drawTrackLane (
        juce::Graphics&,
        juce::Rectangle<int>,
        int trackIndex,
        const juce::String& name
    );

    JADContext& context;

    JADLookAndFeel jadLookAndFeel;

    TransportPanel transportPanel;
    SessionOverviewPanel sessionOverviewPanel;
    SynthVoicePanel synthVoicePanel;

    VirtualKeyboardPanel keyboardPanel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (
        MainComponent
    )
};

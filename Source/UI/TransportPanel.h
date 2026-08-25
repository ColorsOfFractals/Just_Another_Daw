#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_events/juce_events.h>

#include "../Model/TransportState.h"

class TransportPanel :
    public juce::Component,
    private juce::Timer
{
public:
    explicit TransportPanel (
        TransportState& transportState
    );

    ~TransportPanel() override;

    void paint (
        juce::Graphics& g
    ) override;

    void resized() override;

private:
    void timerCallback() override;
    void refreshState();

    TransportState& transport;

    juce::TextButton stopButton { "STOP" };
    juce::TextButton playButton { "PLAY" };
    juce::TextButton recordButton { "REC" };
    juce::TextButton loopButton { "LOOP" };

    juce::Slider tempoSlider;

    juce::Label tempoLabel;
    juce::Label positionLabel;
    juce::Label meterLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (
        TransportPanel
    )
};

#pragma once

#include <juce_events/juce_events.h>
#include <juce_gui_basics/juce_gui_basics.h>

#include <functional>

#include "../Model/TransportState.h"


class MetronomeButton final :
    public juce::TextButton
{
public:
    explicit MetronomeButton (
        const juce::String& text)
        : juce::TextButton (text)
    {
    }

    std::function<void()> onRightClick;

    void mouseDown (
        const juce::MouseEvent& event) override
    {
        if (event.mods.isPopupMenu())
        {
            if (onRightClick)
                onRightClick();

            return;
        }

        juce::TextButton::mouseDown (
            event
        );
    }

    void mouseUp (
        const juce::MouseEvent& event) override
    {
        if (event.mods.isPopupMenu())
            return;

        juce::TextButton::mouseUp (
            event
        );
    }
};


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
    void showMetronomeSettings();

    TransportState& transport;

    juce::TextButton stopButton { "STOP" };
    juce::TextButton playButton { "PLAY" };
    juce::TextButton recordButton { "REC" };
    juce::TextButton loopButton { "LOOP" };

    MetronomeButton metronomeButton { "MET" };

    juce::Slider tempoSlider;

    juce::Label tempoLabel;
    juce::Label positionLabel;
    juce::Label meterLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (
        TransportPanel
    )
};
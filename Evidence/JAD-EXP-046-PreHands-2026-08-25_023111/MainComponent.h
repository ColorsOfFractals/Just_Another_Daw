#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include "../Core/JADContext.h"

#include "JADLookAndFeel.h"
#include "SessionOverviewPanel.h"
#include "SynthVoicePanel.h"
#include "TransportPanel.h"
#include "VirtualKeyboardPanel.h"

class MainComponent :
    public juce::Component,
    private juce::Timer
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

    void mouseDown (
        const juce::MouseEvent&
    ) override;

private:
    enum class DockPage
    {
        keyboard = 0,
        synth,
        mixer
    };

    void timerCallback() override;

    void addTrack();
    void deleteSelectedTrack();

    void selectTrack (
        int index
    );

    void setDockPage (
        DockPage page
    );

    void refreshWorkspace();

    void drawPanel (
        juce::Graphics&,
        juce::Rectangle<int>,
        const juce::String& title,
        juce::Colour accent,
        float alpha = 0.16f
    );

    void drawTrackRack (
        juce::Graphics&,
        juce::Rectangle<int>
    );

    void drawArrangement (
        juce::Graphics&,
        juce::Rectangle<int>
    );

    void drawInspector (
        juce::Graphics&,
        juce::Rectangle<int>
    );

    void drawDock (
        juce::Graphics&,
        juce::Rectangle<int>
    );

    juce::Rectangle<int> getTrackRackBounds() const;
    juce::Rectangle<int> getArrangementBounds() const;
    juce::Rectangle<int> getInspectorBounds() const;
    juce::Rectangle<int> getDockBounds() const;

    JADContext& context;

    JADLookAndFeel jadLookAndFeel;

    TransportPanel transportPanel;
    SessionOverviewPanel sessionOverviewPanel;
    SynthVoicePanel synthVoicePanel;
    VirtualKeyboardPanel keyboardPanel;

    juce::Label projectLabel;
    juce::Label statusLabel;

    juce::TextButton addTrackButton {
        "+ TRACK"
    };

    juce::TextButton deleteTrackButton {
        "- TRACK"
    };

    juce::TextButton keyboardTabButton {
        "KEYBOARD"
    };

    juce::TextButton synthTabButton {
        "SYNTH"
    };

    juce::TextButton mixerTabButton {
        "MIXER"
    };

    int selectedTrackIndex = 0;

    DockPage dockPage =
        DockPage::keyboard;

    double visibleBeats = 32.0;

    juce::Rectangle<int> cachedTrackRack;
    juce::Rectangle<int> cachedArrangement;
    juce::Rectangle<int> cachedInspector;
    juce::Rectangle<int> cachedDock;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (
        MainComponent
    )
};

#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include "../Core/JADContext.h"

#include "JADLookAndFeel.h"
#include "SessionOverviewPanel.h"
#include "SynthVoicePanel.h"
#include "TimelineViewportState.h"
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

    void mouseDrag (
        const juce::MouseEvent&
    ) override;

    void mouseUp (
        const juce::MouseEvent&
    ) override;

private:
    enum class DockPage
    {
        keyboard = 0,
        synth,
        mixer
    };

    enum class DragMode
    {
        none = 0,
        moveClip,
        resizeClip,
        trackGain,
        trackPan
    };

    void timerCallback() override;

    void addTrack();
    void deleteSelectedTrack();

    void selectTrack (
        int index
    );

    void clearClipSelection();

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

    juce::Rectangle<int>
    getTrackRowBounds (
        int trackIndex
    ) const;

    juce::Rectangle<int>
    getTrackGainBounds (
        int trackIndex
    ) const;

    juce::Rectangle<int>
    getTrackPanBounds (
        int trackIndex
    ) const;

    juce::Rectangle<int>
    getTimelineBounds() const;

    juce::Rectangle<int>
    getLaneBounds (
        int trackIndex
    ) const;

    juce::Rectangle<int>
    getClipBounds (
        int trackIndex,
        int clipIndex
    ) const;

    int beatToX (
        double beat
    ) const;

    double xToBeat (
        int x
    ) const;

    void seekTransport (
        double beat
    );

    void beginClipInteraction (
        int trackIndex,
        int clipIndex,
        int mouseX,
        bool resize
    );

    JADContext& context;

    JADLookAndFeel jadLookAndFeel;

    TransportPanel transportPanel;
    SessionOverviewPanel sessionOverviewPanel;
    SynthVoicePanel synthVoicePanel;
    VirtualKeyboardPanel keyboardPanel;

    TimelineViewportState timelineViewport;

    juce::Label projectLabel;
    juce::Label statusLabel;

    juce::TextButton addTrackButton {
        "+ TRACK"
    };

    juce::TextButton deleteTrackButton {
        "- TRACK"
    };

    juce::TextButton scrollLeftButton {
        "<<"
    };

    juce::TextButton scrollRightButton {
        ">>"
    };

    juce::TextButton zoomOutButton {
        "ZOOM -"
    };

    juce::TextButton zoomInButton {
        "ZOOM +"
    };

    juce::TextButton snapButton {
        "SNAP"
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

    int selectedClipTrackIndex = -1;
    int selectedClipIndex = -1;

    DockPage dockPage =
        DockPage::keyboard;

    DragMode dragMode =
        DragMode::none;

    int dragTrackIndex = -1;
    int dragClipIndex = -1;

    int dragAnchorX = 0;

    double dragInitialStartBeat = 0.0;
    double dragInitialLengthBeats = 0.0;

    juce::Rectangle<int> cachedTrackRack;
    juce::Rectangle<int> cachedArrangement;
    juce::Rectangle<int> cachedInspector;
    juce::Rectangle<int> cachedDock;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (
        MainComponent
    )
};

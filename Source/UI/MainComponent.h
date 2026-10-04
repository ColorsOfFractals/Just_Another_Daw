#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include <vector>

#include "../Core/JADContext.h"

#include "JADLookAndFeel.h"
#include "AudioSettingsPanel.h"
#include "PluginLibraryPanel.h"
#include "SessionOverviewPanel.h"
#include "StartupOverlay.h"
#include "SynthVoicePanel.h"
#include "TimelineViewportState.h"
#include "TransportPanel.h"
#include "VirtualKeyboardPanel.h"

class MusicalTitleButton final :
    public juce::Button
{
public:
    MusicalTitleButton()
        : juce::Button (
            "Open JAD command deck"
        )
    {
        setMouseCursor (
            juce::MouseCursor::PointingHandCursor
        );

        setTooltip (
            "Open the JAD command deck"
        );
    }

    void paintButton (
        juce::Graphics& g,
        bool isMouseOverButton,
        bool isButtonDown) override
    {
        auto bounds =
            getLocalBounds()
                .toFloat()
                .reduced (1.5f);

        if (isButtonDown)
            bounds = bounds.translated (0.0f, 1.5f);

        const auto glowAlpha =
            isMouseOverButton
                ? 0.72f
                : 0.34f;

        g.setColour (
            juce::Colour::fromRGB (
                0,
                232,
                255
            ).withAlpha (glowAlpha)
        );

        g.drawRoundedRectangle (
            bounds.expanded (
                isMouseOverButton
                    ? 1.0f
                    : 0.0f
            ),
            13.0f,
            isMouseOverButton
                ? 2.2f
                : 1.2f
        );

        juce::ColourGradient buttonGradient (
            isButtonDown
                ? juce::Colour::fromRGB (125, 20, 120)
                : juce::Colour::fromRGB (233, 27, 130),
            bounds.getTopLeft(),
            isMouseOverButton
                ? juce::Colour::fromRGB (0, 176, 203)
                : juce::Colour::fromRGB (90, 49, 180),
            bounds.getBottomRight(),
            false
        );

        buttonGradient.addColour (
            0.52,
            juce::Colour::fromRGB (
                119,
                54,
                181
            )
        );

        g.setGradientFill (
            buttonGradient
        );

        g.fillRoundedRectangle (
            bounds,
            12.0f
        );

        g.setColour (
            juce::Colours::black.withAlpha (
                isButtonDown
                    ? 0.20f
                    : 0.10f
            )
        );

        g.fillRoundedRectangle (
            bounds.reduced (
                5.0f,
                6.0f
            ),
            8.0f
        );

        auto content =
            bounds.toNearestInt()
                .reduced (10, 2);

        auto leftNote =
            content.removeFromLeft (26);

        auto rightNote =
            content.removeFromRight (26);

        g.setFont (
            juce::FontOptions (20.0f)
                .withStyle ("Bold")
        );

        g.setColour (
            juce::Colour::fromRGB (
                85,
                245,
                255
            )
        );

        g.drawText (
            juce::String::charToString (
                0x266B
            ),
            leftNote,
            juce::Justification::centred
        );

        g.setColour (
            juce::Colour::fromRGB (
                255,
                116,
                197
            )
        );

        g.drawText (
            juce::String::charToString (
                0x266A
            ),
            rightNote,
            juce::Justification::centred
        );

        g.setColour (
            juce::Colours::white
                .withAlpha (
                    isButtonDown
                        ? 0.84f
                        : 1.0f
                )
        );

        g.setFont (
            juce::FontOptions (
                isMouseOverButton
                    ? 22.0f
                    : 21.0f
            ).withStyle ("Bold")
        );

        g.drawText (
            "JAD",
            content,
            juce::Justification::centred
        );
    }
};

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
        mixer,
        plugins
    };

    enum class DragMode
    {
        none = 0,
        moveClip,
        resizeClip,
        trackGain,
        trackPan,
        playhead
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

    void showJadCommandDeck();

    void quantizeSelectedClip();
    void undoLastQuantize();
    void redoLastQuantize();
    void refreshEditCommandState();

    ClipModel* getSelectedClip() noexcept;

    void refreshWorkspace();
    void updateLiveRecordingRegion();

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
    getMixerStripBounds (
        int trackIndex
    );

    juce::Rectangle<int>
    getMixerPanBounds (
        int trackIndex
    );

    juce::Rectangle<int>
    getMixerButtonBounds (
        int trackIndex,
        int buttonIndex
    );

    juce::Rectangle<int>
    getMixerMasterMuteBounds();

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
    StartupOverlay startupOverlay;
    AudioSettingsPanel audioSettingsPanel;
    SynthVoicePanel synthVoicePanel;
    PluginLibraryPanel pluginLibraryPanel;
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

    juce::TextButton audioSettingsButton {
        "PREFERENCES"
    };

    juce::TextButton undoButton {
        "UNDO"
    };

    juce::TextButton redoButton {
        "REDO"
    };

    juce::ComboBox quantizeGridBox;

    juce::TextButton quantizeButton {
        "QUANTIZE"
    };

    MusicalTitleButton jadMenuButton;

    juce::TextButton goToHomeButton {
        "GO TO HOME"
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

    juce::TextButton pluginsTabButton {
        "PLUGINS"
    };

    int selectedTrackIndex = 0;

    int selectedClipTrackIndex = -1;
    int selectedClipIndex = -1;

    std::vector<ClipModel::MidiEvent>
        quantizeUndoEvents;

    std::vector<ClipModel::MidiEvent>
        quantizeRedoEvents;

    ClipModel::ClipId quantizeHistoryClipId = 0;

    bool recordingWasActive = false;
    int activeRecordingTrackIndex = -1;
    int activeRecordingClipIndex = -1;
    double activeRecordingStartBeat = 0.0;

    DockPage dockPage =
        DockPage::keyboard;

    DragMode dragMode =
        DragMode::none;

    int dragTrackIndex = -1;
    int dragClipIndex = -1;

    int dragAnchorX = 0;

    double dragInitialStartBeat = 0.0;
    double dragInitialLengthBeats = 0.0;

    float dragInitialPan = 0.0f;

    juce::Rectangle<int> cachedTrackRack;
    juce::Rectangle<int> cachedArrangement;
    juce::Rectangle<int> cachedInspector;
    juce::Rectangle<int> cachedDock;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (
        MainComponent
    )
};


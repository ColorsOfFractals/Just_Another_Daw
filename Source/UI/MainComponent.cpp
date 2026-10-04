#include "MainComponent.h"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <functional>
#include <memory>
#include <string>

namespace
{
class JadCommandDeckComponent final :
    public juce::Component
{
public:
    JadCommandDeckComponent()
    {
        setSize (520, 310);

        titleLabel.setText (
            "JAD COMMAND DECK",
            juce::dontSendNotification
        );

        titleLabel.setFont (
            juce::FontOptions (20.0f)
                .withStyle ("Bold")
        );

        subtitleLabel.setText (
            "Just Another DAW  |  make music, not menus",
            juce::dontSendNotification
        );

        subtitleLabel.setColour (
            juce::Label::textColourId,
            juce::Colours::white.withAlpha (0.58f)
        );

        newButton.onClick = [this]
        {
            showComingSoon ("NEW SESSION");
        };

        openButton.onClick = [this]
        {
            showComingSoon ("OPEN SESSION");
        };

        saveButton.onClick = [this]
        {
            showComingSoon ("SAVE SESSION");
        };

        saveAsButton.onClick = [this]
        {
            showComingSoon ("SAVE SESSION AS");
        };

        preferencesButton.onClick = [this]
        {
            runAndDismiss (onPreferences);
        };

        updatesButton.onClick = [this]
        {
            runAndDismiss (onUpdates);
        };

        aboutButton.onClick = [this]
        {
            runAndDismiss (onAbout);
        };

        githubButton.onClick = [this]
        {
            runAndDismiss (onGitHub);
        };

        closeButton.onClick = [this]
        {
            dismiss();
        };

        for (
            auto* component :
            std::initializer_list<juce::Component*> {
                &titleLabel,
                &subtitleLabel,
                &newButton,
                &openButton,
                &saveButton,
                &saveAsButton,
                &preferencesButton,
                &updatesButton,
                &aboutButton,
                &githubButton,
                &closeButton
            }
        )
        {
            addAndMakeVisible (component);
        }
    }

    void paint (
        juce::Graphics& g) override
    {
        const auto bounds =
            getLocalBounds().toFloat();

        juce::ColourGradient background (
            juce::Colour::fromRGB (55, 25, 91),
            bounds.getTopLeft(),
            juce::Colour::fromRGB (7, 66, 88),
            bounds.getBottomRight(),
            false
        );

        background.addColour (
            0.52,
            juce::Colour::fromRGB (32, 48, 111)
        );

        g.setGradientFill (background);
        g.fillRoundedRectangle (
            bounds,
            14.0f
        );

        g.setColour (
            juce::Colour::fromRGB (255, 31, 133)
                .withAlpha (0.88f)
        );

        g.fillRoundedRectangle (
            juce::Rectangle<float> (
                20.0f,
                66.0f,
                bounds.getWidth() - 40.0f,
                3.0f
            ),
            1.5f
        );

        g.setColour (
            juce::Colours::white.withAlpha (0.09f)
        );

        g.drawRoundedRectangle (
            bounds.reduced (1.0f),
            14.0f,
            1.0f
        );
    }

    void resized() override
    {
        auto area =
            getLocalBounds().reduced (20);

        auto header =
            area.removeFromTop (48);

        closeButton.setBounds (
            header.removeFromRight (34)
                .reduced (3)
        );

        titleLabel.setBounds (
            header.removeFromTop (26)
        );

        subtitleLabel.setBounds (
            header
        );

        area.removeFromTop (18);

        constexpr int gap = 10;

        const auto buttonWidth =
            (area.getWidth() - gap) / 2;

        auto row1 =
            area.removeFromTop (48);

        newButton.setBounds (
            row1.removeFromLeft (buttonWidth)
                .reduced (2)
        );

        row1.removeFromLeft (gap);

        openButton.setBounds (
            row1.reduced (2)
        );

        area.removeFromTop (6);

        auto row2 =
            area.removeFromTop (48);

        saveButton.setBounds (
            row2.removeFromLeft (buttonWidth)
                .reduced (2)
        );

        row2.removeFromLeft (gap);

        saveAsButton.setBounds (
            row2.reduced (2)
        );

        area.removeFromTop (12);

        auto row3 =
            area.removeFromTop (42);

        preferencesButton.setBounds (
            row3.removeFromLeft (buttonWidth)
                .reduced (2)
        );

        row3.removeFromLeft (gap);

        updatesButton.setBounds (
            row3.reduced (2)
        );

        area.removeFromTop (6);

        auto row4 =
            area.removeFromTop (42);

        aboutButton.setBounds (
            row4.removeFromLeft (buttonWidth)
                .reduced (2)
        );

        row4.removeFromLeft (gap);

        githubButton.setBounds (
            row4.reduced (2)
        );
    }

    std::function<void()> onPreferences;
    std::function<void()> onUpdates;
    std::function<void()> onAbout;
    std::function<void()> onGitHub;

private:
    void dismiss()
    {
        if (
            auto* callout =
                findParentComponentOfClass<
                    juce::CallOutBox
                >()
        )
        {
            callout->dismiss();
        }
    }

    void runAndDismiss (
        const std::function<void()>& action)
    {
        if (action)
            action();

        dismiss();
    }

    void showComingSoon (
        const juce::String& feature)
    {
        juce::AlertWindow::showMessageBoxAsync (
            juce::MessageBoxIconType::InfoIcon,
            feature,
            "COMING SOON\n\nThe command deck is online. "
            "Session persistence is the next engine layer."
        );
    }

    juce::Label titleLabel;
    juce::Label subtitleLabel;

    juce::TextButton newButton {
        "NEW SESSION"
    };

    juce::TextButton openButton {
        "OPEN SESSION"
    };

    juce::TextButton saveButton {
        "SAVE"
    };

    juce::TextButton saveAsButton {
        "SAVE AS"
    };

    juce::TextButton preferencesButton {
        "PREFERENCES"
    };

    juce::TextButton updatesButton {
        "CHECK FOR UPDATES"
    };

    juce::TextButton aboutButton {
        "ABOUT JAD"
    };

    juce::TextButton githubButton {
        "GITHUB"
    };

    juce::TextButton closeButton {
        "X"
    };
};
}

MainComponent::MainComponent (
    JADContext& contextToUse)
    : context (contextToUse),
      transportPanel (
          context.getSessionState()
              .getTransportState()
      ),
      sessionOverviewPanel (
          context.getSessionState()
      ),
      synthVoicePanel (
          context.getAudioSystem()
      ),
      audioSettingsPanel (
          context.getAudioSystem(),
          context.getMidiSystem(),
          jadLookAndFeel
      ),
      pluginLibraryPanel (
          context.getPluginCatalog(),
          context.getAudioSystem(),
          context.getSessionState(),
           jadLookAndFeel
      ),
      keyboardPanel (
          context.getAudioSystem(),
          context.getMidiSystem(),
          jadLookAndFeel
      )
{
    setLookAndFeel (
        &jadLookAndFeel
    );

    setSize (
        1540,
        980
    );

    setWantsKeyboardFocus (
        true
    );

    projectLabel.setText (
        "FIRST JAD SESSION",
        juce::dontSendNotification
    );

    projectLabel.setFont (
        juce::FontOptions (15.0f)
            .withStyle ("Bold")
    );

    projectLabel.setJustificationType (
        juce::Justification::centredLeft
    );

    statusLabel.setText (
        "PROJECT - LIVE",
        juce::dontSendNotification
    );

    statusLabel.setFont (
        juce::FontOptions (11.0f)
    );

    statusLabel.setJustificationType (
        juce::Justification::centredRight
    );

    addAndMakeVisible (
        audioSettingsPanel
    );

    audioSettingsPanel.setVisible (
        false
    );

    audioSettingsButton.onClick =
        [this]
        {
            const bool shouldShow =
                ! audioSettingsPanel.isVisible();

            audioSettingsPanel.setVisible (
                shouldShow
            );

            if (shouldShow)
            {
                audioSettingsPanel.toFront (
                    true
                );
            }

            resized();
            repaint();
        };

    quantizeGridBox.addItem ("1/4", 1);
    quantizeGridBox.addItem ("1/8", 2);
    quantizeGridBox.addItem ("1/16", 3);
    quantizeGridBox.addItem ("1/32", 4);

    quantizeGridBox.setSelectedId (
        3,
        juce::dontSendNotification
    );

    quantizeButton.onClick =
        [this]
        {
            quantizeSelectedClip();
        };

    undoButton.onClick =
        [this]
        {
            undoLastQuantize();
        };

    redoButton.onClick =
        [this]
        {
            redoLastQuantize();
        };

    refreshEditCommandState();

    // AUDIO SETTINGS CLOSE CALLBACK
    audioSettingsPanel.onClose =
        [this]
        {
            audioSettingsPanel.setVisible (
                false
            );

            resized();
            repaint();
        };

    // GO TO HOME TRANSPORT COMMAND
    goToHomeButton.onClick =
        [this]
        {
            context.getSessionState()
                .getTransportState()
                .returnToStart();

            repaint();
        };
    // JAD COMMAND DECK BUTTON
    jadMenuButton.onClick =
        [this]
        {
            showJadCommandDeck();
        };
    pluginLibraryPanel.onTrackAssigned =
        [this] (int trackIndex)
        {
            selectTrack (trackIndex);
        };

    context.getAudioSystem().setMidiTargetTrack (
        selectedTrackIndex
    );

    addTrackButton.onClick =
        [this]
        {
            addTrack();
        };

    deleteTrackButton.onClick =
        [this]
        {
            deleteSelectedTrack();
        };

    scrollLeftButton.onClick =
        [this]
        {
            timelineViewport.scrollLeft();
            repaint();
        };

    scrollRightButton.onClick =
        [this]
        {
            timelineViewport.scrollRight();
            repaint();
        };

    zoomOutButton.onClick =
        [this]
        {
            timelineViewport.zoomOut();
            repaint();
        };

    zoomInButton.onClick =
        [this]
        {
            timelineViewport.zoomIn();
            repaint();
        };

    snapButton.setClickingTogglesState (
        true
    );

    snapButton.setToggleState (
        timelineViewport.isSnapEnabled(),
        juce::dontSendNotification
    );

    snapButton.onClick =
        [this]
        {
            timelineViewport.setSnapEnabled (
                snapButton.getToggleState()
            );

            repaint();
        };

    keyboardTabButton.onClick =
        [this]
        {
            setDockPage (
                DockPage::keyboard
            );
        };

    synthTabButton.onClick =
        [this]
        {
            setDockPage (
                DockPage::synth
            );
        };

    mixerTabButton.onClick =
        [this]
        {
            setDockPage (
                DockPage::mixer
            );
        };

    pluginsTabButton.onClick =
        [this]
        {
            setDockPage (
                DockPage::plugins
            );
        };

    addAndMakeVisible (
        transportPanel
    );

    addAndMakeVisible (
        sessionOverviewPanel
    );

    addAndMakeVisible (
        synthVoicePanel
    );

    addAndMakeVisible (
        pluginLibraryPanel
    );

    addAndMakeVisible (
        keyboardPanel
    );

    addAndMakeVisible (
        projectLabel
    );

    addAndMakeVisible (
        statusLabel
    );

    addAndMakeVisible (
        addTrackButton
    );

    addAndMakeVisible (
        deleteTrackButton
    );

    addAndMakeVisible (
        scrollLeftButton
    );

    addAndMakeVisible (
        scrollRightButton
    );

    addAndMakeVisible (
        zoomOutButton
    );

    addAndMakeVisible (
        zoomInButton
    );

    addAndMakeVisible (
        snapButton
    );

    addAndMakeVisible (
        undoButton
    );

    addAndMakeVisible (
        redoButton
    );

    addAndMakeVisible (
        quantizeGridBox
    );

    addAndMakeVisible (
        quantizeButton
    );

    addAndMakeVisible (
        audioSettingsButton
    );

    // JAD COMMAND DECK COMPONENT
    addAndMakeVisible (
        jadMenuButton
    );

    // GO TO HOME COMPONENT
    addAndMakeVisible (
        goToHomeButton
    );
addAndMakeVisible (
        keyboardTabButton
    );

    addAndMakeVisible (
        synthTabButton
    );

    addAndMakeVisible (
        mixerTabButton
    );

    addAndMakeVisible (
        pluginsTabButton
    );

    sessionOverviewPanel.setVisible (
        false
    );

    setDockPage (
        DockPage::keyboard
    );

    startTimerHz (30);
}

void MainComponent::showJadCommandDeck()
{
    auto deck =
        std::make_unique<
            JadCommandDeckComponent
        >();

    deck->onPreferences =
        [this]
        {
            audioSettingsPanel.setVisible (true);
            audioSettingsPanel.toFront (true);
            resized();
            repaint();
        };

    deck->onUpdates =
        []
        {
            juce::URL (
                "https://github.com/ColorsOfFractals/"
                "Just_Another_Daw/releases"
            ).launchInDefaultBrowser();
        };

    deck->onAbout =
        []
        {
            juce::AlertWindow::showMessageBoxAsync (
                juce::MessageBoxIconType::InfoIcon,
                "ABOUT JAD",
                "JUST ANOTHER DAW\n\n"
                "Version 0.1.x development build\n"
                "Open-source under AGPLv3\n\n"
                "Hands on timeline. Make the thing."
            );
        };

    deck->onGitHub =
        []
        {
            juce::URL (
                "https://github.com/ColorsOfFractals/"
                "Just_Another_Daw"
            ).launchInDefaultBrowser();
        };

    juce::CallOutBox::launchAsynchronously (
        std::move (deck),
        jadMenuButton.getScreenBounds(),
        nullptr
    );
}

MainComponent::~MainComponent()
{
    stopTimer();

    setLookAndFeel (
        nullptr
    );
}

void MainComponent::timerCallback()
{
    updateLiveRecordingRegion();
    refreshWorkspace();
}

void MainComponent::updateLiveRecordingRegion()
{
    auto& session =
        context.getSessionState();

    auto& transport =
        session.getTransportState();

    const auto recordingNow =
        transport.isRecording();

    if (recordingNow && ! recordingWasActive)
    {
        activeRecordingTrackIndex = -1;
        activeRecordingClipIndex = -1;

        if (selectedTrackIndex >= 0)
        {
            auto* track =
                session.getTrack (
                    static_cast<std::size_t> (
                        selectedTrackIndex
                    )
                );

            if (
                track != nullptr
                && track->isRecordArmed()
            )
            {
                activeRecordingStartBeat =
                    transport.getPositionInBeats();

                track->clearRecordedMidiEvents();

                track->addClip (
                    "MIDI RECORDING",
                    activeRecordingStartBeat,
                    0.001
                );

                activeRecordingTrackIndex =
                    selectedTrackIndex;

                activeRecordingClipIndex =
                    static_cast<int> (
                        track->getClipCount()
                    ) - 1;

                selectedClipTrackIndex =
                    activeRecordingTrackIndex;

                selectedClipIndex =
                    activeRecordingClipIndex;
            }
        }
    }

    if (
        recordingNow
        && activeRecordingTrackIndex >= 0
        && activeRecordingClipIndex >= 0
    )
    {
        auto* track =
            session.getTrack (
                static_cast<std::size_t> (
                    activeRecordingTrackIndex
                )
            );

        auto* clip =
            track != nullptr
                ? track->getClip (
                    static_cast<std::size_t> (
                        activeRecordingClipIndex
                    )
                )
                : nullptr;

        if (clip != nullptr)
        {
            clip->setLengthBeats (
                juce::jmax (
                    0.001,
                    transport.getPositionInBeats()
                        - activeRecordingStartBeat
                )
            );
        }
    }

    if (! recordingNow && recordingWasActive)
    {
        if (
            activeRecordingTrackIndex >= 0
            && activeRecordingClipIndex >= 0
        )
        {
            auto* track =
                session.getTrack (
                    static_cast<std::size_t> (
                        activeRecordingTrackIndex
                    )
                );

            auto* clip =
                track != nullptr
                    ? track->getClip (
                        static_cast<std::size_t> (
                            activeRecordingClipIndex
                        )
                    )
                    : nullptr;

            if (
                track != nullptr
                && clip != nullptr
            )
            {
                const auto eventCount =
                    track->getRecordedMidiEventCount();

                if (eventCount == 0)
                {
                    track->removeClip (
                        clip->getId()
                    );

                    clearClipSelection();
                }
                else
                {
                    clip->setMidiEvents (
                        track->getRecordedMidiEventsSnapshot()
                    );

                    clip->setName (
                        "MIDI TAKE - "
                            + std::to_string (
                                (eventCount + 1) / 2
                            )
                            + " NOTES"
                    );
                }
            }
        }

        activeRecordingTrackIndex = -1;
        activeRecordingClipIndex = -1;
    }

    recordingWasActive = recordingNow;
}

void MainComponent::refreshWorkspace()
{
    auto& session =
        context.getSessionState();

    const auto count =
        static_cast<int> (
            session.getTrackCount()
        );

    if (count <= 0)
        selectedTrackIndex = -1;

    if (
        count > 0
        && selectedTrackIndex < 0
    )
    {
        selectedTrackIndex = 0;
    }

    if (
        count > 0
        && selectedTrackIndex >= count
    )
    {
        selectedTrackIndex =
            count - 1;
    }

    if (
        selectedClipTrackIndex
        >= count
    )
    {
        clearClipSelection();
    }

    statusLabel.setText (
        "TRACKS "
            + juce::String (count)
            + "  -  "
            + (
                timelineViewport.isSnapEnabled()
                    ? "SNAP 1/4"
                    : "SNAP OFF"
            )
            + "  -  "
            + juce::String (
                timelineViewport.getVisibleBeats(),
                1
            )
            + " BEATS  -  AUDIO "
            + (
                context.getAudioSystem().isReady()
                    ? "ONLINE"
                    : "OFFLINE"
            ),
        juce::dontSendNotification
    );

    repaint();
}

void MainComponent::addTrack()
{
    auto& session =
        context.getSessionState();

    auto& track =
        session.addTrack();

    track.addClip (
        "NEW CLIP",
        timelineViewport.getStartBeat(),
        4.0
    );

    selectedTrackIndex =
        static_cast<int> (
            session.getTrackCount()
        ) - 1;

    context.getAudioSystem().refreshTrackRoutes();
    context.getAudioSystem().setMidiTargetTrack (
        selectedTrackIndex
    );

    clearClipSelection();

    refreshWorkspace();
}

void MainComponent::deleteSelectedTrack()
{
    auto& session =
        context.getSessionState();

    if (selectedTrackIndex < 0)
        return;

    auto* track =
        session.getTrack (
            static_cast<std::size_t> (
                selectedTrackIndex
            )
        );

    if (track == nullptr)
        return;

    const auto removedTrackIndex =
        selectedTrackIndex;

    session.removeTrack (
        track->getId()
    );

    context.getAudioSystem().handleTrackRemoved (
        removedTrackIndex
    );

    clearClipSelection();

    const auto count =
        static_cast<int> (
            session.getTrackCount()
        );

    if (count <= 0)
        selectedTrackIndex = -1;

    if (
        count > 0
        && selectedTrackIndex >= count
    )
    {
        selectedTrackIndex =
            count - 1;
    }

    refreshWorkspace();
}

void MainComponent::selectTrack (
    int index)
{
    const auto count =
        static_cast<int> (
            context.getSessionState()
                .getTrackCount()
        );

    if (
        index < 0
        || index >= count
    )
    {
        return;
    }

    selectedTrackIndex = index;

    context.getAudioSystem().setMidiTargetTrack (
        selectedTrackIndex
    );

    if (
        selectedClipTrackIndex
        != selectedTrackIndex
    )
    {
        clearClipSelection();
    }

    repaint();
}

void MainComponent::clearClipSelection()
{
    selectedClipTrackIndex = -1;
    selectedClipIndex = -1;
}

ClipModel* MainComponent::getSelectedClip() noexcept
{
    if (
        selectedClipTrackIndex < 0
        || selectedClipIndex < 0
    )
    {
        return nullptr;
    }

    auto* track =
        context.getSessionState()
            .getTrack (
                static_cast<std::size_t> (
                    selectedClipTrackIndex
                )
            );

    if (track == nullptr)
        return nullptr;

    return track->getClip (
        static_cast<std::size_t> (
            selectedClipIndex
        )
    );
}


void MainComponent::quantizeSelectedClip()
{
    auto* clip =
        getSelectedClip();

    if (clip == nullptr)
    {
        statusLabel.setText (
            "SELECT A MIDI CLIP TO QUANTIZE",
            juce::dontSendNotification
        );

        return;
    }

    if (clip->getMidiEventCount() == 0)
    {
        statusLabel.setText (
            "SELECTED CLIP HAS NO OWNED MIDI EVENTS",
            juce::dontSendNotification
        );

        return;
    }

    double gridBeats = 0.25;
    juce::String gridName = "1/16";

    switch (quantizeGridBox.getSelectedId())
    {
        case 1:
            gridBeats = 1.0;
            gridName = "1/4";
            break;

        case 2:
            gridBeats = 0.5;
            gridName = "1/8";
            break;

        case 4:
            gridBeats = 0.125;
            gridName = "1/32";
            break;

        case 3:
        default:
            gridBeats = 0.25;
            gridName = "1/16";
            break;
    }

    quantizeUndoEvents =
        clip->getMidiEventsSnapshot();

    quantizeRedoEvents.clear();

    quantizeHistoryClipId =
        clip->getId();

    const auto noteCount =
        clip->quantizeMidi (
            gridBeats
        );

    statusLabel.setText (
        "QUANTIZED "
            + juce::String (
                static_cast<int> (
                    noteCount
                )
            )
            + " NOTES - "
            + gridName,
        juce::dontSendNotification
    );

    refreshEditCommandState();
    repaint();
}


void MainComponent::undoLastQuantize()
{
    auto* clip =
        getSelectedClip();

    if (
        clip == nullptr
        || clip->getId()
            != quantizeHistoryClipId
        || quantizeUndoEvents.empty()
    )
    {
        return;
    }

    quantizeRedoEvents =
        clip->getMidiEventsSnapshot();

    clip->setMidiEvents (
        quantizeUndoEvents
    );

    quantizeUndoEvents.clear();

    statusLabel.setText (
        "QUANTIZE UNDONE",
        juce::dontSendNotification
    );

    refreshEditCommandState();
    repaint();
}


void MainComponent::redoLastQuantize()
{
    auto* clip =
        getSelectedClip();

    if (
        clip == nullptr
        || clip->getId()
            != quantizeHistoryClipId
        || quantizeRedoEvents.empty()
    )
    {
        return;
    }

    quantizeUndoEvents =
        clip->getMidiEventsSnapshot();

    clip->setMidiEvents (
        quantizeRedoEvents
    );

    quantizeRedoEvents.clear();

    statusLabel.setText (
        "QUANTIZE REDONE",
        juce::dontSendNotification
    );

    refreshEditCommandState();
    repaint();
}


void MainComponent::refreshEditCommandState()
{
    auto* clip =
        getSelectedClip();

    const auto selectedHistoryMatches =
        clip != nullptr
        && clip->getId()
            == quantizeHistoryClipId;

    undoButton.setEnabled (
        selectedHistoryMatches
        && ! quantizeUndoEvents.empty()
    );

    redoButton.setEnabled (
        selectedHistoryMatches
        && ! quantizeRedoEvents.empty()
    );

    quantizeButton.setEnabled (
        clip != nullptr
        && clip->getMidiEventCount() > 0
    );
}

void MainComponent::setDockPage (
    DockPage page)
{
    dockPage = page;

    keyboardPanel.setVisible (
        dockPage == DockPage::keyboard
    );

    synthVoicePanel.setVisible (
        dockPage == DockPage::synth
    );

    pluginLibraryPanel.setVisible (
        dockPage == DockPage::plugins
    );

    keyboardTabButton.setToggleState (
        dockPage == DockPage::keyboard,
        juce::dontSendNotification
    );

    synthTabButton.setToggleState (
        dockPage == DockPage::synth,
        juce::dontSendNotification
    );

    mixerTabButton.setToggleState (
        dockPage == DockPage::mixer,
        juce::dontSendNotification
    );

    pluginsTabButton.setToggleState (
        dockPage == DockPage::plugins,
        juce::dontSendNotification
    );

    resized();
    repaint();
}

void MainComponent::drawPanel (
    juce::Graphics& g,
    juce::Rectangle<int> area,
    const juce::String& title,
    juce::Colour accent,
    float alpha)
{
    if (area.isEmpty())
        return;

    const auto bounds =
        area.toFloat();

    juce::ColourGradient glass (
        accent.withAlpha (
            alpha + 0.08f
        ),
        bounds.getTopLeft(),
        jadLookAndFeel.colourB()
            .withAlpha (
                alpha * 0.45f
            ),
        bounds.getBottomRight(),
        false
    );

    g.setGradientFill (
        glass
    );

    g.fillRoundedRectangle (
        bounds,
        14.0f
    );

    g.setColour (
        accent.withAlpha (0.50f)
    );

    g.drawRoundedRectangle (
        bounds.reduced (0.75f),
        14.0f,
        1.2f
    );

    if (title.isEmpty())
        return;

    auto titleArea =
        area.removeFromTop (28)
            .reduced (10, 0);

    g.setColour (
        juce::Colours::white
            .withAlpha (0.78f)
    );

    g.setFont (
        juce::FontOptions (11.0f)
            .withStyle ("Bold")
    );

    g.drawText (
        title,
        titleArea,
        juce::Justification::centredLeft
    );
}

juce::Rectangle<int>
MainComponent::getTrackRowBounds (
    int trackIndex) const
{
    auto body =
        cachedTrackRack.reduced (8);

    body.removeFromTop (30);

    constexpr int rowHeight = 72;
    constexpr int gap = 5;

    const auto y =
        body.getY()
        + trackIndex
        * (rowHeight + gap);

    return {
        body.getX(),
        y,
        body.getWidth(),
        rowHeight
    };
}

juce::Rectangle<int>
MainComponent::getTrackGainBounds (
    int trackIndex) const
{
    auto row =
        getTrackRowBounds (
            trackIndex
        )
        .reduced (8, 5);

    row.removeFromTop (28);

    auto gain =
        row.removeFromTop (15);

    gain.removeFromLeft (82);

    return gain;
}

juce::Rectangle<int>
MainComponent::getTrackPanBounds (
    int trackIndex) const
{
    auto row =
        getTrackRowBounds (
            trackIndex
        )
        .reduced (8, 5);

    row.removeFromTop (45);

    auto pan =
        row.removeFromTop (15);

    pan.removeFromLeft (82);

    return pan;
}

juce::Rectangle<int>
MainComponent::getMixerStripBounds (
    int trackIndex)
{
    if (dockPage != DockPage::mixer)
        return {};

    auto console =
        cachedDock.reduced (14);

    console.removeFromTop (46);

    auto& session =
        context.getSessionState();

    const auto count =
        static_cast<int> (
            session.getTrackCount()
        );

    if (
        trackIndex < 0
        || trackIndex >= count
        || count <= 0
    )
    {
        return {};
    }

    constexpr int gap = 6;
    constexpr int masterWidth = 112;

    console.removeFromRight (
        juce::jmin (
            masterWidth,
            console.getWidth() / 4
        )
    );

    console.removeFromRight (gap);

    const auto stripWidth =
        juce::jmax (
            76,
            (
                console.getWidth()
                - gap * juce::jmax (
                    0,
                    count - 1
                )
            )
            / juce::jmax (
                1,
                count
            )
        );

    const auto x =
        console.getX()
        + trackIndex
            * (
                stripWidth + gap
            );

    if (
        x + stripWidth
        > console.getRight()
    )
    {
        return {};
    }

    return {
        x,
        console.getY(),
        stripWidth,
        console.getHeight()
    };
}


juce::Rectangle<int>
MainComponent::getMixerPanBounds (
    int trackIndex)
{
    auto content =
        getMixerStripBounds (
            trackIndex
        )
        .reduced (7);

    if (content.isEmpty())
        return {};

    content.removeFromTop (22);
    content.removeFromTop (24);
    content.removeFromTop (4);

    return content.removeFromTop (38);
}


juce::Rectangle<int>
MainComponent::getMixerButtonBounds (
    int trackIndex,
    int buttonIndex)
{
    auto content =
        getMixerStripBounds (
            trackIndex
        )
        .reduced (7);

    if (
        content.isEmpty()
        || buttonIndex < 0
        || buttonIndex > 2
    )
    {
        return {};
    }

    content.removeFromTop (22);
    content.removeFromTop (24);
    content.removeFromTop (4);
    content.removeFromTop (38);

    auto buttons =
        content.removeFromTop (23);

    const auto buttonWidth =
        juce::jmax (
            16,
            (
                buttons.getWidth()
                - 4
            ) / 3
        );

    if (buttonIndex == 0)
        return buttons.removeFromLeft (
            buttonWidth
        );

    buttons.removeFromLeft (
        buttonWidth
    );

    buttons.removeFromLeft (2);

    if (buttonIndex == 1)
        return buttons.removeFromLeft (
            buttonWidth
        );

    buttons.removeFromLeft (
        buttonWidth
    );

    buttons.removeFromLeft (2);

    return buttons;
}


juce::Rectangle<int>
MainComponent::getMixerMasterMuteBounds()
{
    if (dockPage != DockPage::mixer)
        return {};

    auto console =
        cachedDock.reduced (14);

    console.removeFromTop (46);

    constexpr int masterWidth = 112;

    auto masterArea =
        console.removeFromRight (
            juce::jmin (
                masterWidth,
                console.getWidth() / 4
            )
        );

    auto content =
        masterArea.reduced (8);

    content.removeFromTop (24);
    content.removeFromTop (24);
    content.removeFromTop (6);

    return content.removeFromTop (22);
}

juce::Rectangle<int>
MainComponent::getTimelineBounds() const
{
    auto body =
        cachedArrangement.reduced (8);

    body.removeFromTop (28);
    body.removeFromTop (30);

    return body;
}

juce::Rectangle<int>
MainComponent::getLaneBounds (
    int trackIndex) const
{
    auto timeline =
        getTimelineBounds();

    const auto count =
        static_cast<int> (
            context.getSessionState()
                .getTrackCount()
        );

    if (
        count <= 0
        || trackIndex < 0
        || trackIndex >= count
    )
    {
        return {};
    }

    constexpr int gap = 5;

    const auto available =
        timeline.getHeight()
        - gap * juce::jmax (
            0,
            count - 1
        );

    const auto laneHeight =
        juce::jmax (
            28,
            available
                / juce::jmax (
                    1,
                    count
                )
        );

    const auto y =
        timeline.getY()
        + trackIndex
        * (laneHeight + gap);

    return {
        timeline.getX(),
        y,
        timeline.getWidth(),
        laneHeight
    };
}

int MainComponent::beatToX (
    double beat) const
{
    const auto timeline =
        getTimelineBounds();

    if (timeline.getWidth() <= 0)
        return timeline.getX();

    const auto fraction =
        (
            beat
            - timelineViewport.getStartBeat()
        )
        / timelineViewport.getVisibleBeats();

    return timeline.getX()
        + static_cast<int> (
            fraction
            * timeline.getWidth()
        );
}

double MainComponent::xToBeat (
    int x) const
{
    const auto timeline =
        getTimelineBounds();

    if (timeline.getWidth() <= 0)
        return timelineViewport.getStartBeat();

    const auto fraction =
        juce::jlimit (
            0.0,
            1.0,
            static_cast<double> (
                x - timeline.getX()
            )
            / static_cast<double> (
                timeline.getWidth()
            )
        );

    return timelineViewport.getStartBeat()
        + fraction
        * timelineViewport.getVisibleBeats();
}

juce::Rectangle<int>
MainComponent::getClipBounds (
    int trackIndex,
    int clipIndex) const
{
    const auto lane =
        getLaneBounds (
            trackIndex
        );

    if (lane.isEmpty())
        return {};

    auto* track =
        context.getSessionState()
            .getTrack (
                static_cast<std::size_t> (
                    trackIndex
                )
            );

    if (track == nullptr)
        return {};

    if (
        clipIndex < 0
        || clipIndex
            >= static_cast<int> (
                track->getClipCount()
            )
    )
    {
        return {};
    }

    auto* clip =
        track->getClip (
            static_cast<std::size_t> (
                clipIndex
            )
        );

    if (clip == nullptr)
        return {};

    const auto visibleStart =
        timelineViewport.getStartBeat();

    const auto visibleEnd =
        timelineViewport.getEndBeat();

    const auto clipStart =
        clip->getStartBeat();

    const auto clipEnd =
        clipStart
        + clip->getLengthBeats();

    if (
        clipEnd <= visibleStart
        || clipStart >= visibleEnd
    )
    {
        return {};
    }

    const auto start =
        juce::jmax (
            visibleStart,
            clipStart
        );

    const auto end =
        juce::jmin (
            visibleEnd,
            clipEnd
        );

    const auto x1 =
        beatToX (start);

    const auto x2 =
        beatToX (end);

    return {
        x1 + 3,
        lane.getY() + 5,
        juce::jmax (
            8,
            x2 - x1 - 6
        ),
        juce::jmax (
            8,
            lane.getHeight() - 10
        )
    };
}

void MainComponent::drawTrackRack (
    juce::Graphics& g,
    juce::Rectangle<int> area)
{
    drawPanel (
        g,
        area,
        "TRACK RACK",
        jadLookAndFeel.colourA(),
        0.16f
    );

    auto& session =
        context.getSessionState();

    const auto count =
        static_cast<int> (
            session.getTrackCount()
        );

    if (count <= 0)
        return;

    for (
        int index = 0;
        index < count;
        ++index
    )
    {
        const auto row =
            getTrackRowBounds (
                index
            );

        if (
            row.getBottom()
            > cachedTrackRack.getBottom()
        )
        {
            break;
        }

        auto* track =
            session.getTrack (
                static_cast<std::size_t> (
                    index
                )
            );

        if (track == nullptr)
            continue;

        const auto colour =
            jadLookAndFeel.trackColour (
                index
            );

        const auto selected =
            index
            == selectedTrackIndex;

        juce::ColourGradient gradient (
            colour.withAlpha (
                selected ? 0.72f : 0.28f
            ),
            row.getTopLeft().toFloat(),
            jadLookAndFeel.colourB()
                .withAlpha (
                    selected ? 0.30f : 0.10f
                ),
            row.getBottomRight().toFloat(),
            false
        );

        g.setGradientFill (
            gradient
        );

        g.fillRoundedRectangle (
            row.toFloat(),
            9.0f
        );

        g.setColour (
            colour.withAlpha (
                selected ? 0.96f : 0.38f
            )
        );

        g.drawRoundedRectangle (
            row.toFloat(),
            9.0f,
            selected ? 2.0f : 1.0f
        );

        auto inner =
            row.reduced (8, 5);

        auto title =
            inner.removeFromTop (25);

        g.setColour (
            juce::Colours::white
        );

        g.setFont (
            juce::FontOptions (12.0f)
                .withStyle ("Bold")
        );

        g.drawText (
            juce::String (
                track->getName()
            ),
            title,
            juce::Justification::centredLeft
        );

        // TRACK TITLE TOGGLE GEOMETRY
        auto controlLine =
            title;

        controlLine.removeFromLeft (
            juce::jmin (
                92,
                controlLine.getWidth()
            )
        );

        auto mute =
            controlLine.removeFromLeft (23);

        controlLine.removeFromLeft (3);

        auto solo =
            controlLine.removeFromLeft (23);

        controlLine.removeFromLeft (3);

        auto arm =
            controlLine.removeFromLeft (23);

        const auto drawToggle =
            [&g, colour] (
                juce::Rectangle<int> bounds,
                const juce::String& text,
                bool enabled)
            {
                g.setColour (
                    enabled
                        ? colour.withAlpha (0.90f)
                        : juce::Colours::black
                            .withAlpha (0.34f)
                );

                g.fillRoundedRectangle (
                    bounds.toFloat(),
                    4.0f
                );

                g.setColour (
                    juce::Colours::white
                        .withAlpha (
                            enabled
                                ? 1.0f
                                : 0.56f
                        )
                );

                g.setFont (
                    juce::FontOptions (9.0f)
                        .withStyle ("Bold")
                );

                g.drawText (
                    text,
                    bounds,
                    juce::Justification::centred
                );
            };

        drawToggle (
            mute,
            "M",
            track->isMuted()
        );

        drawToggle (
            solo,
            "S",
            track->isSolo()
        );

        drawToggle (
            arm,
            "R",
            track->isRecordArmed()
        );

        const auto gainBounds =
            getTrackGainBounds (
                index
            );

        const auto panBounds =
            getTrackPanBounds (
                index
            );

        g.setColour (
            juce::Colours::black
                .withAlpha (0.42f)
        );

        g.fillRoundedRectangle (
            gainBounds.toFloat(),
            3.0f
        );

        g.fillRoundedRectangle (
            panBounds.toFloat(),
            3.0f
        );

        const auto gainFraction =
            juce::jlimit (
                0.0f,
                1.0f,
                track->getGain()
                    / 2.0f
            );

        auto gainFill =
            gainBounds;

        gainFill.setWidth (
            static_cast<int> (
                gainBounds.getWidth()
                * gainFraction
            )
        );

        g.setColour (
            colour.withAlpha (0.82f)
        );

        g.fillRoundedRectangle (
            gainFill.toFloat(),
            3.0f
        );

        const auto panFraction =
            (
                track->getPan()
                + 1.0f
            ) * 0.5f;

        const auto panX =
            panBounds.getX()
            + static_cast<int> (
                panFraction
                * panBounds.getWidth()
            );

        g.setColour (
            juce::Colours::white
                .withAlpha (0.86f)
        );

        g.drawVerticalLine (
            panX,
            static_cast<float> (
                panBounds.getY()
            ),
            static_cast<float> (
                panBounds.getBottom()
            )
        );

        g.setFont (7.5f);

        g.setColour (
            juce::Colours::white
                .withAlpha (0.45f)
        );

        g.drawText (
            "GAIN",
            gainBounds.translated (
                -38,
                0
            ),
            juce::Justification::centredLeft
        );

        g.drawText (
            "PAN",
            panBounds.translated (
                -38,
                0
            ),
            juce::Justification::centredLeft
        );
    }
}

void MainComponent::drawArrangement (
    juce::Graphics& g,
    juce::Rectangle<int> area)
{
    drawPanel (
        g,
        area,
        "ARRANGEMENT - CLICK TO SEEK - DRAG CLIPS - EDGE TO RESIZE",
        jadLookAndFeel.colourD(),
        0.11f
    );

    auto rulerBody =
        area.reduced (8);

    rulerBody.removeFromTop (28);

    auto ruler =
        rulerBody.removeFromTop (30);

    const auto timeline =
        getTimelineBounds();

    g.setColour (
        juce::Colours::black
            .withAlpha (0.28f)
    );

    g.fillRoundedRectangle (
        ruler.toFloat(),
        7.0f
    );

    const auto firstBeat =
        static_cast<int> (
            std::floor (
                timelineViewport.getStartBeat()
            )
        );

    const auto lastBeat =
        static_cast<int> (
            std::ceil (
                timelineViewport.getEndBeat()
            )
        );

    for (
        int beat = firstBeat;
        beat <= lastBeat;
        ++beat
    )
    {
        const auto x =
            beatToX (
                static_cast<double> (
                    beat
                )
            );

        const auto bar =
            beat % 4 == 0;

        g.setColour (
            (
                bar
                    ? jadLookAndFeel.colourD()
                    : juce::Colours::white
            )
            .withAlpha (
                bar ? 0.46f : 0.10f
            )
        );

        g.drawVerticalLine (
            x,
            static_cast<float> (
                ruler.getY()
            ),
            static_cast<float> (
                timeline.getBottom()
            )
        );

        if (! bar)
            continue;

        g.setColour (
            juce::Colours::white
                .withAlpha (0.70f)
        );

        g.setFont (9.5f);

        g.drawText (
            juce::String (
                (beat / 4) + 1
            ),
            x + 4,
            ruler.getY(),
            38,
            ruler.getHeight(),
            juce::Justification::centredLeft
        );
    }

    auto& session =
        context.getSessionState();

    const auto count =
        static_cast<int> (
            session.getTrackCount()
        );

    for (
        int trackIndex = 0;
        trackIndex < count;
        ++trackIndex
    )
    {
        const auto lane =
            getLaneBounds (
                trackIndex
            );

        if (lane.isEmpty())
            continue;

        const auto colour =
            jadLookAndFeel.trackColour (
                trackIndex
            );

        const auto selected =
            trackIndex
            == selectedTrackIndex;

        g.setColour (
            colour.withAlpha (
                selected ? 0.18f : 0.07f
            )
        );

        g.fillRoundedRectangle (
            lane.toFloat(),
            7.0f
        );

        g.setColour (
            colour.withAlpha (
                selected ? 0.48f : 0.18f
            )
        );

        g.drawRoundedRectangle (
            lane.toFloat(),
            7.0f,
            selected ? 1.5f : 0.8f
        );

        auto* track =
            session.getTrack (
                static_cast<std::size_t> (
                    trackIndex
                )
            );

        if (track == nullptr)
            continue;

        for (
            int clipIndex = 0;
            clipIndex
                < static_cast<int> (
                    track->getClipCount()
                );
            ++clipIndex
        )
        {
            auto* clip =
                track->getClip (
                    static_cast<std::size_t> (
                        clipIndex
                    )
                );

            if (clip == nullptr)
                continue;

            const auto clipBounds =
                getClipBounds (
                    trackIndex,
                    clipIndex
                );

            if (clipBounds.isEmpty())
                continue;

            const auto clipSelected =
                trackIndex
                    == selectedClipTrackIndex
                && clipIndex
                    == selectedClipIndex;

            juce::ColourGradient clipGradient (
                colour.withAlpha (
                    clip->isMuted()
                        ? 0.28f
                        : 0.90f
                ),
                clipBounds
                    .getTopLeft()
                    .toFloat(),
                jadLookAndFeel.colourB()
                    .withAlpha (
                        clip->isMuted()
                            ? 0.16f
                            : 0.54f
                    ),
                clipBounds
                    .getBottomRight()
                    .toFloat(),
                false
            );

            g.setGradientFill (
                clipGradient
            );

            g.fillRoundedRectangle (
                clipBounds.toFloat(),
                6.0f
            );

            if (clipSelected)
            {
                g.setColour (
                    juce::Colours::white
                );

                g.drawRoundedRectangle (
                    clipBounds
                        .toFloat()
                        .expanded (2.0f),
                    7.0f,
                    2.5f
                );
            }

            auto resizeHandle =
                clipBounds.withLeft (
                    clipBounds.getRight() - 8
                );

            g.setColour (
                juce::Colours::white
                    .withAlpha (
                        clipSelected
                            ? 0.85f
                            : 0.30f
                    )
            );

            g.fillRoundedRectangle (
                resizeHandle
                    .reduced (2, 5)
                    .toFloat(),
                2.0f
            );

            g.setColour (
                juce::Colours::white
                    .withAlpha (
                        clip->isMuted()
                            ? 0.42f
                            : 0.90f
                    )
            );

            g.setFont (
                juce::FontOptions (10.0f)
                    .withStyle ("Bold")
            );

            g.drawText (
                juce::String (
                    clip->getName()
                ),
                clipBounds.reduced (7),
                juce::Justification::centredLeft
            );
        }
    }

    const auto playheadBeat =
        context.getSessionState()
            .getTransportState()
            .getPositionInBeats();

    if (
        playheadBeat
            >= timelineViewport.getStartBeat()
        && playheadBeat
            <= timelineViewport.getEndBeat()
    )
    {
        const auto x =
            beatToX (
                playheadBeat
            );

        g.setColour (
            juce::Colours::white
                .withAlpha (0.94f)
        );

        g.drawVerticalLine (
            x,
            static_cast<float> (
                ruler.getY()
            ),
            static_cast<float> (
                timeline.getBottom()
            )
        );

        g.setColour (
            jadLookAndFeel.colourA()
        );

        juce::Path head;

        head.addTriangle (
            static_cast<float> (
                x - 5
            ),
            static_cast<float> (
                ruler.getY()
            ),
            static_cast<float> (
                x + 5
            ),
            static_cast<float> (
                ruler.getY()
            ),
            static_cast<float> (x),
            static_cast<float> (
                ruler.getY() + 8
            )
        );

        g.fillPath (
            head
        );
    }
}

void MainComponent::drawInspector (
    juce::Graphics& g,
    juce::Rectangle<int> area)
{
    drawPanel (
        g,
        area,
        "INSPECTOR",
        jadLookAndFeel.colourB(),
        0.15f
    );

    auto body =
        area.reduced (14);

    body.removeFromTop (34);

    auto& session =
        context.getSessionState();

    if (selectedTrackIndex < 0)
    {
        g.setColour (
            juce::Colours::white
                .withAlpha (0.40f)
        );

        g.drawText (
            "NO TRACK SELECTED",
            body,
            juce::Justification::centred
        );

        return;
    }

    auto* track =
        session.getTrack (
            static_cast<std::size_t> (
                selectedTrackIndex
            )
        );

    if (track == nullptr)
        return;

    const auto colour =
        jadLookAndFeel.trackColour (
            selectedTrackIndex
        );

    g.setColour (
        colour
    );

    g.fillRoundedRectangle (
        body.removeFromTop (5)
            .toFloat(),
        2.5f
    );

    body.removeFromTop (12);

    g.setColour (
        juce::Colours::white
    );

    g.setFont (
        juce::FontOptions (16.0f)
            .withStyle ("Bold")
    );

    g.drawText (
        juce::String (
            track->getName()
        ),
        body.removeFromTop (28),
        juce::Justification::centredLeft
    );

    const auto drawValue =
        [&g, &body, colour] (
            const juce::String& label,
            const juce::String& value)
        {
            auto row =
                body.removeFromTop (30);

            g.setColour (
                juce::Colours::white
                    .withAlpha (0.42f)
            );

            g.setFont (9.0f);

            g.drawText (
                label,
                row.removeFromTop (12),
                juce::Justification::centredLeft
            );

            g.setColour (
                colour.withAlpha (0.94f)
            );

            g.setFont (
                juce::FontOptions (12.0f)
                    .withStyle ("Bold")
            );

            g.drawText (
                value,
                row,
                juce::Justification::centredLeft
            );

            body.removeFromTop (5);
        };

    drawValue (
        "GAIN",
        juce::String (
            track->getGain(),
            2
        )
    );

    drawValue (
        "PAN",
        juce::String (
            track->getPan(),
            2
        )
    );

    juce::String trackState;

    if (track->isMuted())
        trackState += "MUTE ";

    if (track->isSolo())
        trackState += "SOLO ";

    if (track->isRecordArmed())
        trackState += "ARM ";

    if (trackState.isEmpty())
        trackState = "READY";

    drawValue (
        "TRACK STATE",
        trackState.trim()
    );

    if (
        selectedClipTrackIndex
            != selectedTrackIndex
        || selectedClipIndex < 0
    )
    {
        drawValue (
            "CLIPS",
            juce::String (
                static_cast<int> (
                    track->getClipCount()
                )
            )
        );

        drawValue (
            "MIDI EVENTS",
            juce::String (
                static_cast<int> (
                    track->getRecordedMidiEventCount()
                )
            )
        );

        return;
    }

    auto* clip =
        track->getClip (
            static_cast<std::size_t> (
                selectedClipIndex
            )
        );

    if (clip == nullptr)
        return;

    body.removeFromTop (4);

    g.setColour (
        juce::Colours::white
            .withAlpha (0.20f)
    );

    g.drawHorizontalLine (
        body.getY(),
        static_cast<float> (
            body.getX()
        ),
        static_cast<float> (
            body.getRight()
        )
    );

    body.removeFromTop (10);

    drawValue (
        "SELECTED CLIP",
        juce::String (
            clip->getName()
        )
    );

    drawValue (
        "START BEAT",
        juce::String (
            clip->getStartBeat(),
            2
        )
    );

    drawValue (
        "LENGTH",
        juce::String (
            clip->getLengthBeats(),
            2
        )
    );
}

void MainComponent::drawDock (
    juce::Graphics& g,
    juce::Rectangle<int> area)
{
    drawPanel (
        g,
        area,
        "",
        jadLookAndFeel.colourC(),
        0.10f
    );

    if (dockPage != DockPage::mixer)
        return;

    auto console =
        area.reduced (14);

    console.removeFromTop (46);

    auto& session =
        context.getSessionState();

    auto& audio =
        context.getAudioSystem();

    const auto count =
        static_cast<int> (
            session.getTrackCount()
        );

    if (count <= 0)
        return;

    constexpr int gap = 6;
    constexpr int masterWidth = 112;

    auto masterArea =
        console.removeFromRight (
            juce::jmin (
                masterWidth,
                console.getWidth() / 4
            )
        );

    console.removeFromRight (gap);

    const auto stripWidth =
        juce::jmax (
            76,
            (
                console.getWidth()
                - gap * juce::jmax (0, count - 1)
            )
            / juce::jmax (1, count)
        );

    const auto drawButton =
        [&g] (
            juce::Rectangle<int> bounds,
            const juce::String& label,
            bool active,
            juce::Colour colour)
        {
            g.setColour (
                active
                    ? colour.withAlpha (0.95f)
                    : juce::Colours::black.withAlpha (0.42f)
            );

            g.fillRoundedRectangle (
                bounds.toFloat(),
                3.0f
            );

            g.setColour (
                active
                    ? juce::Colours::white
                    : juce::Colours::white.withAlpha (0.45f)
            );

            g.setFont (
                juce::FontOptions (9.0f)
                    .withStyle ("Bold")
            );

            g.drawText (
                label,
                bounds,
                juce::Justification::centred
            );
        };

    const auto drawFader =
        [&g] (
            juce::Rectangle<int> bounds,
            float gain,
            juce::Colour colour)
        {
            const auto fraction =
                juce::jlimit (
                    0.0f,
                    1.0f,
                    gain * 0.5f
                );

            const auto centreX =
                bounds.getCentreX();

            const auto trackTop =
                bounds.getY() + 8;

            const auto trackBottom =
                bounds.getBottom() - 8;

            g.setColour (
                juce::Colours::black.withAlpha (0.72f)
            );

            g.fillRoundedRectangle (
                juce::Rectangle<float> (
                    static_cast<float> (centreX - 3),
                    static_cast<float> (trackTop),
                    6.0f,
                    static_cast<float> (
                        trackBottom - trackTop
                    )
                ),
                3.0f
            );

            for (int tick = 0; tick <= 10; ++tick)
            {
                const auto y =
                    trackTop
                    + (
                        trackBottom - trackTop
                    ) * tick / 10;

                const auto major =
                    tick % 5 == 0;

                g.setColour (
                    juce::Colours::white.withAlpha (
                        major ? 0.34f : 0.16f
                    )
                );

                g.drawHorizontalLine (
                    y,
                    static_cast<float> (
                        bounds.getX()
                        + (major ? 5 : 9)
                    ),
                    static_cast<float> (
                        bounds.getRight()
                        - (major ? 5 : 9)
                    )
                );
            }

            const auto handleY =
                trackBottom
                - static_cast<int> (
                    fraction
                    * (
                        trackBottom - trackTop
                    )
                );

            auto handle =
                juce::Rectangle<int> (
                    bounds.getX() + 4,
                    handleY - 6,
                    bounds.getWidth() - 8,
                    12
                );

            g.setColour (
                colour.withAlpha (0.98f)
            );

            g.fillRoundedRectangle (
                handle.toFloat(),
                3.0f
            );

            g.setColour (
                juce::Colours::white.withAlpha (0.74f)
            );

            g.drawHorizontalLine (
                handle.getCentreY(),
                static_cast<float> (
                    handle.getX() + 4
                ),
                static_cast<float> (
                    handle.getRight() - 4
                )
            );
        };

    for (
        int index = 0;
        index < count;
        ++index
    )
    {
        if (console.getWidth() < stripWidth)
            break;

        auto strip =
            console.removeFromLeft (
                stripWidth
            );

        console.removeFromLeft (gap);

        auto* track =
            session.getTrack (
                static_cast<std::size_t> (
                    index
                )
            );

        if (track == nullptr)
            continue;

        const auto colour =
            jadLookAndFeel.trackColour (
                index
            );

        g.setColour (
            juce::Colour::fromRGB (
                15,
                18,
                29
            ).withAlpha (0.92f)
        );

        g.fillRoundedRectangle (
            strip.toFloat(),
            6.0f
        );

        g.setColour (
            colour.withAlpha (0.62f)
        );

        g.drawRoundedRectangle (
            strip.toFloat(),
            6.0f,
            selectedTrackIndex == index
                ? 2.0f
                : 1.0f
        );

        auto content =
            strip.reduced (7);

        auto title =
            content.removeFromTop (22);

        g.setColour (
            juce::Colours::white
        );

        g.setFont (
            juce::FontOptions (10.0f)
                .withStyle ("Bold")
        );

        g.drawText (
            track->getName(),
            title,
            juce::Justification::centred,
            true
        );

        auto insert =
            content.removeFromTop (24)
                .reduced (1, 2);

        g.setColour (
            juce::Colours::black
                .withAlpha (0.58f)
        );

        g.fillRoundedRectangle (
            insert.toFloat(),
            3.0f
        );

        auto pluginName =
            audio.getTrackPluginName (
                index
            );

        if (pluginName.isEmpty())
        {
            pluginName =
                index == 0
                    ? "NATIVE"
                    : "NO INSERT";
        }

        g.setColour (
            colour.withAlpha (0.86f)
        );

        g.setFont (8.0f);

        g.drawText (
            pluginName,
            insert.reduced (4, 0),
            juce::Justification::centred,
            true
        );

        content.removeFromTop (4);

        auto panArea =
            content.removeFromTop (38);

        const auto panCentre =
            panArea.getCentre()
                .toFloat();

        constexpr float knobRadius = 12.0f;

        g.setColour (
            juce::Colours::black
                .withAlpha (0.74f)
        );

        g.fillEllipse (
            panCentre.x - knobRadius,
            panCentre.y - knobRadius,
            knobRadius * 2.0f,
            knobRadius * 2.0f
        );

        const auto pan =
            juce::jlimit (
                -1.0f,
                1.0f,
                track->getPan()
            );

        const auto angle =
            juce::MathConstants<float>::pi
            * (
                1.25f
                + 1.5f
                    * (
                        pan + 1.0f
                    ) * 0.5f
            );

        g.setColour (
            colour
        );

        g.drawLine (
            panCentre.x,
            panCentre.y,
            panCentre.x
                + std::cos (angle)
                    * (knobRadius - 3.0f),
            panCentre.y
                + std::sin (angle)
                    * (knobRadius - 3.0f),
            2.0f
        );

        g.setColour (
            juce::Colours::white
                .withAlpha (0.54f)
        );

        g.setFont (7.5f);

        g.drawText (
            "PAN",
            panArea.removeFromBottom (10),
            juce::Justification::centred
        );

        auto buttons =
            content.removeFromTop (23);

        const auto buttonWidth =
            juce::jmax (
                16,
                (
                    buttons.getWidth()
                    - 4
                ) / 3
            );

        auto mute =
            buttons.removeFromLeft (
                buttonWidth
            );

        buttons.removeFromLeft (2);

        auto solo =
            buttons.removeFromLeft (
                buttonWidth
            );

        buttons.removeFromLeft (2);

        auto arm =
            buttons;

        drawButton (
            mute,
            "M",
            track->isMuted(),
            juce::Colour::fromRGB (
                232,
                74,
                94
            )
        );

        drawButton (
            solo,
            "S",
            track->isSolo(),
            juce::Colour::fromRGB (
                239,
                184,
                48
            )
        );

        drawButton (
            arm,
            "R",
            track->isRecordArmed(),
            colour
        );

        content.removeFromTop (4);

        auto footer =
            content.removeFromBottom (19);

        auto valueArea =
            content.removeFromBottom (18);

        auto meterArea =
            content.removeFromRight (
                juce::jmax (
                    12,
                    content.getWidth() / 4
                )
            );

        content.removeFromRight (3);

        auto faderArea =
            content;

        drawFader (
            faderArea,
            track->getGain(),
            colour
        );

        g.setColour (
            juce::Colours::black
                .withAlpha (0.68f)
        );

        g.fillRoundedRectangle (
            meterArea.toFloat(),
            3.0f
        );

        const auto level =
            juce::jlimit (
                0.0f,
                1.0f,
                track->getGain() * 0.5f
            );

        auto meterFill =
            meterArea.reduced (3);

        meterFill.removeFromTop (
            static_cast<int> (
                meterFill.getHeight()
                * (1.0f - level)
            )
        );

        juce::ColourGradient meterGradient (
            juce::Colour::fromRGB (
                42,
                222,
                142
            ),
            meterArea.getBottomLeft()
                .toFloat(),
            juce::Colour::fromRGB (
                255,
                79,
                112
            ),
            meterArea.getTopLeft()
                .toFloat(),
            false
        );

        g.setGradientFill (
            meterGradient
        );

        g.fillRoundedRectangle (
            meterFill.toFloat(),
            2.0f
        );

        const auto gainDb =
            juce::Decibels::gainToDecibels (
                track->getGain(),
                -60.0f
            );

        g.setColour (
            juce::Colours::white
                .withAlpha (0.72f)
        );

        g.setFont (8.0f);

        g.drawText (
            juce::String (
                gainDb,
                1
            ) + " dB",
            valueArea,
            juce::Justification::centred
        );

        g.setColour (
            colour.withAlpha (0.92f)
        );

        g.fillRoundedRectangle (
            footer.toFloat(),
            3.0f
        );

        g.setColour (
            juce::Colours::white
        );

        g.setFont (
            juce::FontOptions (9.0f)
                .withStyle ("Bold")
        );

        g.drawText (
            juce::String (index + 1),
            footer,
            juce::Justification::centred
        );
    }

    auto& master =
        session.getMasterBusState();

    const auto masterColour =
        jadLookAndFeel.colourC();

    g.setColour (
        juce::Colour::fromRGB (
            10,
            12,
            22
        ).withAlpha (0.96f)
    );

    g.fillRoundedRectangle (
        masterArea.toFloat(),
        6.0f
    );

    g.setColour (
        masterColour.withAlpha (0.78f)
    );

    g.drawRoundedRectangle (
        masterArea.toFloat(),
        6.0f,
        1.5f
    );

    auto masterContent =
        masterArea.reduced (8);

    g.setColour (
        juce::Colours::white
    );

    g.setFont (
        juce::FontOptions (11.0f)
            .withStyle ("Bold")
    );

    g.drawText (
        "MASTER",
        masterContent.removeFromTop (24),
        juce::Justification::centred
    );

    auto outputSlot =
        masterContent.removeFromTop (24)
            .reduced (1, 2);

    g.setColour (
        juce::Colours::black
            .withAlpha (0.58f)
    );

    g.fillRoundedRectangle (
        outputSlot.toFloat(),
        3.0f
    );

    g.setColour (
        masterColour.withAlpha (0.88f)
    );

    g.setFont (8.0f);

    g.drawText (
        "MAIN OUT",
        outputSlot,
        juce::Justification::centred
    );

    masterContent.removeFromTop (6);

    auto muteButton =
        masterContent.removeFromTop (22);

    drawButton (
        muteButton,
        "MUTE",
        master.isMuted(),
        juce::Colour::fromRGB (
            232,
            74,
            94
        )
    );

    masterContent.removeFromTop (6);

    auto masterValue =
        masterContent.removeFromBottom (20);

    auto masterFooter =
        masterContent.removeFromBottom (20);

    auto masterMeter =
        masterContent.removeFromRight (22);

    masterContent.removeFromRight (5);

    drawFader (
        masterContent,
        master.getGain(),
        masterColour
    );

    g.setColour (
        juce::Colours::black
            .withAlpha (0.72f)
    );

    g.fillRoundedRectangle (
        masterMeter.toFloat(),
        3.0f
    );

    auto masterFill =
        masterMeter.reduced (4);

    const auto masterLevel =
        juce::jlimit (
            0.0f,
            1.0f,
            master.getGain() * 0.5f
        );

    masterFill.removeFromTop (
        static_cast<int> (
            masterFill.getHeight()
            * (1.0f - masterLevel)
        )
    );

    g.setColour (
        masterColour.withAlpha (0.94f)
    );

    g.fillRoundedRectangle (
        masterFill.toFloat(),
        2.0f
    );

    const auto masterDb =
        juce::Decibels::gainToDecibels (
            master.getGain(),
            -60.0f
        );

    g.setColour (
        juce::Colours::white
            .withAlpha (0.74f)
    );

    g.setFont (8.0f);

    g.drawText (
        juce::String (
            masterDb,
            1
        ) + " dB",
        masterValue,
        juce::Justification::centred
    );

    g.setColour (
        masterColour.withAlpha (0.92f)
    );

    g.fillRoundedRectangle (
        masterFooter.toFloat(),
        3.0f
    );

    g.setColour (
        juce::Colours::white
    );

    g.setFont (
        juce::FontOptions (8.5f)
            .withStyle ("Bold")
    );

    g.drawText (
        "OUT",
        masterFooter,
        juce::Justification::centred
    );
}

void MainComponent::paint (
    juce::Graphics& g)
{
    const auto bounds =
        getLocalBounds()
            .toFloat();

    g.fillAll (
        juce::Colour::fromRGB (
            5,
            4,
            14
        )
    );

    juce::ColourGradient background (
        jadLookAndFeel.colourA()
            .withAlpha (0.31f),
        bounds.getTopLeft(),
        jadLookAndFeel.colourC()
            .withAlpha (0.18f),
        bounds.getBottomRight(),
        false
    );

    background.addColour (
        0.32,
        jadLookAndFeel.colourB()
            .withAlpha (0.20f)
    );

    background.addColour (
        0.67,
        jadLookAndFeel.colourD()
            .withAlpha (0.16f)
    );

    background.addColour (
        0.88,
        juce::Colour::fromRGB (
            8,
            5,
            22
        )
    );

    g.setGradientFill (
        background
    );

    g.fillRect (
        bounds
    );

    auto header =
        getLocalBounds()
            .removeFromTop (66)
            .reduced (18, 7);

    juce::ColourGradient headerGradient (
        jadLookAndFeel.colourA()
            .withAlpha (0.82f),
        header.getTopLeft().toFloat(),
        jadLookAndFeel.colourD()
            .withAlpha (0.48f),
        header.getBottomRight().toFloat(),
        false
    );

    g.setGradientFill (
        headerGradient
    );

    g.fillRoundedRectangle (
        header.toFloat(),
        16.0f
    );

    g.setColour (
        juce::Colours::white
    );

    g.setFont (
        juce::FontOptions (25.0f)
            .withStyle ("Bold")
    );

    // JAD TITLE NOW OWNED BY BUTTON

    g.setFont (10.5f);

    g.drawText (
        "JUST ANOTHER DAW - HANDS ON TIMELINE - 047 SEED PLANTED",
        header.reduced (18),
        juce::Justification::centredRight
    );

    drawTrackRack (
        g,
        cachedTrackRack
    );

    drawArrangement (
        g,
        cachedArrangement
    );

    drawInspector (
        g,
        cachedInspector
    );

    drawDock (
        g,
        cachedDock
    );
}

void MainComponent::resized()
{
    // JAD COMMAND DECK GEOMETRY
    jadMenuButton.setBounds (
        20,
        11,
        142,
        44
    );

    // EXP-050E.2D AUDIO COCKPIT GEOMETRY

    const int cockpitWidth =
        juce::jmax (
            320,
            juce::jmin (
                620,
                getWidth() - 40
            )
        );

    const int cockpitHeight =
        juce::jmax (
            300,
            juce::jmin (
                700,
                getHeight() - 90
            )
        );

    audioSettingsPanel.setBounds (
        getWidth() - cockpitWidth - 20,
        54,
        cockpitWidth,
        cockpitHeight
    );

    auto area =
        getLocalBounds();

    area.removeFromTop (68);

    auto commandBar =
        area.removeFromTop (46)
            .reduced (18, 3);

    projectLabel.setBounds (
        commandBar.removeFromLeft (235)
    );

    commandBar.removeFromLeft (6);

    addTrackButton.setBounds (
        commandBar.removeFromLeft (82)
            .reduced (2)
    );

    deleteTrackButton.setBounds (
        commandBar.removeFromLeft (82)
            .reduced (2)
    );

    commandBar.removeFromLeft (8);

    scrollLeftButton.setBounds (
        commandBar.removeFromLeft (42)
            .reduced (2)
    );

    scrollRightButton.setBounds (
        commandBar.removeFromLeft (42)
            .reduced (2)
    );

    zoomOutButton.setBounds (
        commandBar.removeFromLeft (78)
            .reduced (2)
    );

    zoomInButton.setBounds (
        commandBar.removeFromLeft (78)
            .reduced (2)
    );

    snapButton.setBounds (
        commandBar.removeFromLeft (66)
            .reduced (2)
    );

    // EXP-EDIT-001 COMMAND SHELF
    auto statusArea =
        commandBar.removeFromRight (
            juce::jmin (
                210,
                commandBar.getWidth()
            )
        );

    statusLabel.setBounds (
        statusArea
    );

    commandBar.removeFromRight (6);

    audioSettingsButton.setBounds (
        commandBar.removeFromRight (
            juce::jmin (
                132,
                commandBar.getWidth()
            )
        ).reduced (2)
    );

    commandBar.removeFromLeft (8);

    undoButton.setBounds (
        commandBar.removeFromLeft (
            juce::jmin (
                58,
                commandBar.getWidth()
            )
        ).reduced (2)
    );

    redoButton.setBounds (
        commandBar.removeFromLeft (
            juce::jmin (
                58,
                commandBar.getWidth()
            )
        ).reduced (2)
    );

    commandBar.removeFromLeft (4);

    quantizeGridBox.setBounds (
        commandBar.removeFromLeft (
            juce::jmin (
                72,
                commandBar.getWidth()
            )
        ).reduced (2)
    );

    quantizeButton.setBounds (
        commandBar.removeFromLeft (
            juce::jmin (
                96,
                commandBar.getWidth()
            )
        ).reduced (2)
    );

    auto transportArea =
        area.removeFromTop (82)
            .reduced (18, 4);

    transportPanel.setBounds (
        transportArea
    );

    area.reduce (
        18,
        7
    );

    // EXP-PLUGIN-UI-001
    // The lower workspace now receives enough room for a real
    // multi-row plugin cabinet instead of a single device strip.
    const int dockHeight =
        juce::jlimit (
            270,
            410,
            area.getHeight() * 40 / 100
        );

    cachedDock =
        area.removeFromBottom (
            dockHeight
        );

    area.removeFromBottom (10);

    const auto rackWidth =
        juce::jlimit (
            190,
            260,
            area.getWidth() / 5
        );

    const auto inspectorWidth =
        juce::jlimit (
            190,
            260,
            area.getWidth() / 5
        );

    cachedTrackRack =
        area.removeFromLeft (
            rackWidth
        );

    area.removeFromLeft (10);

    cachedInspector =
        area.removeFromRight (
            inspectorWidth
        );

    area.removeFromRight (10);

    cachedArrangement =
        area;

    // GO TO HOME ARRANGEMENT GEOMETRY
    auto arrangementHeader =
        cachedArrangement.reduced (8);

    arrangementHeader.setHeight (30);

    arrangementHeader.removeFromLeft (
        juce::jmin (
            420,
            juce::jmax (
                0,
                arrangementHeader.getWidth() - 120
            )
        )
    );

    goToHomeButton.setBounds (
        arrangementHeader.removeFromLeft (112)
            .reduced (2, 3)
    );

    auto dockControls =
        cachedDock.reduced (12);

    auto tabs =
        dockControls.removeFromTop (38);

    keyboardTabButton.setBounds (
        tabs.removeFromLeft (110)
            .reduced (2)
    );

    synthTabButton.setBounds (
        tabs.removeFromLeft (90)
            .reduced (2)
    );

    mixerTabButton.setBounds (
        tabs.removeFromLeft (90)
            .reduced (2)
    );

    pluginsTabButton.setBounds (
        tabs.removeFromLeft (92)
            .reduced (2)
    );

    dockControls.removeFromTop (4);

    keyboardPanel.setBounds (
        dockControls
    );

    synthVoicePanel.setBounds (
        dockControls
    );

    pluginLibraryPanel.setBounds (
        dockControls
    );

    sessionOverviewPanel.setBounds (
        {}
    );
}

void MainComponent::seekTransport (
    double beat)
{
    auto& transport =
        context.getSessionState()
            .getTransportState();

    const auto snapped =
        timelineViewport.snapBeat (
            beat
        );

    transport.setPositionInBeats (
        snapped
    );

    const auto sampleRate =
        context.getAudioSystem()
            .getSampleRate();

    if (sampleRate <= 0.0)
        return;

    const auto tempo =
        transport.getTempo();

    if (tempo <= 0.0)
        return;

    const auto seconds =
        snapped
        * 60.0
        / tempo;

    transport.setPositionInSamples (
        static_cast<std::int64_t> (
            seconds * sampleRate
        )
    );
}

void MainComponent::beginClipInteraction (
    int trackIndex,
    int clipIndex,
    int mouseX,
    bool resize)
{
    auto* track =
        context.getSessionState()
            .getTrack (
                static_cast<std::size_t> (
                    trackIndex
                )
            );

    if (track == nullptr)
        return;

    auto* clip =
        track->getClip (
            static_cast<std::size_t> (
                clipIndex
            )
        );

    if (clip == nullptr)
        return;

    selectedTrackIndex =
        trackIndex;

    selectedClipTrackIndex =
        trackIndex;

    selectedClipIndex =
        clipIndex;

    dragTrackIndex =
        trackIndex;

    dragClipIndex =
        clipIndex;

    dragAnchorX =
        mouseX;

    dragInitialStartBeat =
        clip->getStartBeat();

    dragInitialLengthBeats =
        clip->getLengthBeats();

    dragMode =
        resize
            ? DragMode::resizeClip
            : DragMode::moveClip;

    repaint();
}

void MainComponent::mouseDown (
    const juce::MouseEvent& event)
{
    const auto point =
        event.getPosition();

    auto& session =
        context.getSessionState();

    const auto count =
        static_cast<int> (
            session.getTrackCount()
        );

    // MIXER CONTROL HIT TEST
    if (
        dockPage == DockPage::mixer
        && cachedDock.contains (point)
    )
    {
        for (
            int index = 0;
            index < count;
            ++index
        )
        {
            const auto strip =
                getMixerStripBounds (
                    index
                );

            if (! strip.contains (point))
                continue;

            selectTrack (index);

            auto* track =
                session.getTrack (
                    static_cast<std::size_t> (
                        index
                    )
                );

            if (track == nullptr)
                return;

            if (
                getMixerButtonBounds (
                    index,
                    0
                )
                .contains (point)
            )
            {
                track->setMuted (
                    ! track->isMuted()
                );

                repaint();
                return;
            }

            if (
                getMixerButtonBounds (
                    index,
                    1
                )
                .contains (point)
            )
            {
                track->setSolo (
                    ! track->isSolo()
                );

                repaint();
                return;
            }

            if (
                getMixerButtonBounds (
                    index,
                    2
                )
                .contains (point)
            )
            {
                track->setRecordArmed (
                    ! track->isRecordArmed()
                );

                repaint();
                return;
            }

            if (
                getMixerPanBounds (
                    index
                )
                .contains (point)
            )
            {
                dragMode =
                    DragMode::trackPan;

                dragTrackIndex =
                    index;

                dragAnchorX =
                    event.x;

                dragInitialPan =
                    track->getPan();

                repaint();
                return;
            }

            repaint();
            return;
        }

        const auto masterMute =
            getMixerMasterMuteBounds();

        if (masterMute.contains (point))
        {
            auto& master =
                session.getMasterBusState();

            master.setMuted (
                ! master.isMuted()
            );

            repaint();
            return;
        }

        return;
    }

    if (
        cachedTrackRack.contains (
            point
        )
    )
    {
        for (
            int index = 0;
            index < count;
            ++index
        )
        {
            const auto row =
                getTrackRowBounds (
                    index
                );

            if (! row.contains (point))
                continue;

            selectTrack (
                index
            );

            auto* track =
                session.getTrack (
                    static_cast<std::size_t> (
                        index
                    )
                );

            if (track == nullptr)
                return;

            auto controls =
                row.reduced (8, 5);

            // TRACK TITLE TOGGLE HITBOXES
            auto toggleLine =
                controls.removeFromTop (25);

            toggleLine.removeFromLeft (
                juce::jmin (
                    92,
                    toggleLine.getWidth()
                )
            );

            auto mute =
                toggleLine.removeFromLeft (23);

            toggleLine.removeFromLeft (3);

            auto solo =
                toggleLine.removeFromLeft (23);

            toggleLine.removeFromLeft (3);

            auto arm =
                toggleLine.removeFromLeft (23);

            if (mute.contains (point))
            {
                track->setMuted (
                    ! track->isMuted()
                );

                repaint();
                return;
            }

            if (solo.contains (point))
            {
                track->setSolo (
                    ! track->isSolo()
                );

                repaint();
                return;
            }

            if (arm.contains (point))
            {
                track->setRecordArmed (
                    ! track->isRecordArmed()
                );

                repaint();
                return;
            }

            const auto gain =
                getTrackGainBounds (
                    index
                );

            if (gain.contains (point))
            {
                dragMode =
                    DragMode::trackGain;

                dragTrackIndex =
                    index;

                mouseDrag (
                    event
                );

                return;
            }

            const auto pan =
                getTrackPanBounds (
                    index
                );

            if (pan.contains (point))
            {
                dragMode =
                    DragMode::trackPan;

                dragTrackIndex =
                    index;

                mouseDrag (
                    event
                );

                return;
            }

            return;
        }

        return;
    }

    const auto timeline =
        getTimelineBounds();

    // PLAYHEAD GRAB HANDLE
    const auto& transport =
        context.getSessionState()
            .getTransportState();

    const auto playheadX =
        beatToX (
            transport.getPositionInBeats()
        );

    // The visible triangle is deliberately tiny.
    // This larger invisible target makes it pleasant to grab.
    const juce::Rectangle<int> playheadGrabArea {
        playheadX - 14,
        timeline.getY() - 38,
        28,
        38
    };

    if (
        playheadGrabArea.contains (point)
    )
    {
        dragMode =
            DragMode::playhead;

        seekTransport (
            xToBeat (
                point.x
            )
        );

        repaint();
        return;
    }

    // CLICKABLE TIMELINE RULER
    const juce::Rectangle<int> timelineRulerArea {
        timeline.getX(),
        timeline.getY() - 30,
        timeline.getWidth(),
        30
    };

    if (
        timelineRulerArea.contains (
            point
        )
    )
    {
        dragMode =
            DragMode::playhead;

        seekTransport (
            xToBeat (
                point.x
            )
        );

        repaint();
        return;
    }
    if (! timeline.contains (point))
        return;

    for (
        int trackIndex = 0;
        trackIndex < count;
        ++trackIndex
    )
    {
        auto* track =
            session.getTrack (
                static_cast<std::size_t> (
                    trackIndex
                )
            );

        if (track == nullptr)
            continue;

        for (
            int clipIndex = 0;
            clipIndex
                < static_cast<int> (
                    track->getClipCount()
                );
            ++clipIndex
        )
        {
            const auto clipBounds =
                getClipBounds (
                    trackIndex,
                    clipIndex
                );

            if (
                ! clipBounds.contains (
                    point
                )
            )
            {
                continue;
            }

            const auto resize =
                point.x
                >= clipBounds.getRight()
                    - 10;

            beginClipInteraction (
                trackIndex,
                clipIndex,
                point.x,
                resize
            );

            return;
        }
    }

    clearClipSelection();

    for (
        int trackIndex = 0;
        trackIndex < count;
        ++trackIndex
    )
    {
        if (
            getLaneBounds (
                trackIndex
            )
            .contains (
                point
            )
        )
        {
            selectTrack (
                trackIndex
            );

            break;
        }
    }

    seekTransport (
        xToBeat (
            point.x
        )
    );

    repaint();
}

void MainComponent::mouseDrag (
    const juce::MouseEvent& event)
{
    auto& session =
        context.getSessionState();

    // PLAYHEAD LIVE SCRUB
    if (
        dragMode
        == DragMode::playhead
    )
    {
        seekTransport (
            xToBeat (
                event.x
            )
        );

        repaint();
        return;
    }

    if (
        dragMode
        == DragMode::trackGain
    )
    {
        auto* track =
            session.getTrack (
                static_cast<std::size_t> (
                    dragTrackIndex
                )
            );

        if (track == nullptr)
            return;

        const auto bounds =
            getTrackGainBounds (
                dragTrackIndex
            );

        if (bounds.getWidth() <= 0)
            return;

        const auto fraction =
            juce::jlimit (
                0.0f,
                1.0f,
                static_cast<float> (
                    event.x
                    - bounds.getX()
                )
                / static_cast<float> (
                    bounds.getWidth()
                )
            );

        track->setGain (
            fraction * 2.0f
        );

        repaint();
        return;
    }

    if (
        dragMode
        == DragMode::trackPan
    )
    {
        auto* track =
            session.getTrack (
                static_cast<std::size_t> (
                    dragTrackIndex
                )
            );

        if (track == nullptr)
            return;

        // MIXER PAN DRAG
        if (dockPage == DockPage::mixer)
        {
            constexpr float dragRange = 120.0f;

            const auto delta =
                static_cast<float> (
                    event.x
                    - dragAnchorX
                )
                / dragRange;

            track->setPan (
                juce::jlimit (
                    -1.0f,
                    1.0f,
                    dragInitialPan
                        + delta * 2.0f
                )
            );

            repaint();
            return;
        }

        const auto bounds =
            getTrackPanBounds (
                dragTrackIndex
            );

        if (bounds.getWidth() <= 0)
            return;

        const auto fraction =
            juce::jlimit (
                0.0f,
                1.0f,
                static_cast<float> (
                    event.x
                    - bounds.getX()
                )
                / static_cast<float> (
                    bounds.getWidth()
                )
            );

        track->setPan (
            fraction * 2.0f
            - 1.0f
        );

        repaint();
        return;
    }

    if (
        dragMode
            != DragMode::moveClip
        && dragMode
            != DragMode::resizeClip
    )
    {
        return;
    }

    auto* track =
        session.getTrack (
            static_cast<std::size_t> (
                dragTrackIndex
            )
        );

    if (track == nullptr)
        return;

    auto* clip =
        track->getClip (
            static_cast<std::size_t> (
                dragClipIndex
            )
        );

    if (clip == nullptr)
        return;

    const auto timeline =
        getTimelineBounds();

    if (timeline.getWidth() <= 0)
        return;

    const auto deltaPixels =
        event.x
        - dragAnchorX;

    const auto deltaBeats =
        static_cast<double> (
            deltaPixels
        )
        / static_cast<double> (
            timeline.getWidth()
        )
        * timelineViewport.getVisibleBeats();

    if (
        dragMode
        == DragMode::moveClip
    )
    {
        clip->setStartBeat (
            timelineViewport.snapBeat (
                dragInitialStartBeat
                + deltaBeats
            )
        );

        repaint();
        return;
    }

    const auto minimum =
        timelineViewport.isSnapEnabled()
            ? timelineViewport.getSnapBeats()
            : 0.05;

    const auto length =
        juce::jmax (
            minimum,
            dragInitialLengthBeats
                + deltaBeats
        );

    auto snappedLength =
        length;

    if (
        timelineViewport.isSnapEnabled()
    )
    {
        snappedLength =
            std::round (
                length
                / timelineViewport
                    .getSnapBeats()
            )
            * timelineViewport
                .getSnapBeats();

        snappedLength =
            juce::jmax (
                minimum,
                snappedLength
            );
    }

    clip->setLengthBeats (
        snappedLength
    );

    repaint();
}

void MainComponent::mouseUp (
    const juce::MouseEvent&)
{
    dragMode =
        DragMode::none;

    dragTrackIndex = -1;
    dragClipIndex = -1;
}






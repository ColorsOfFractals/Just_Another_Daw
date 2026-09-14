#include "MainComponent.h"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <string>

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
        audioSettingsButton
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

        auto controlLine =
            inner.removeFromTop (17);

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

    if (
        dockPage
        != DockPage::mixer
    )
    {
        return;
    }

    auto body =
        area.reduced (18);

    body.removeFromTop (48);

    auto& session =
        context.getSessionState();

    const auto count =
        static_cast<int> (
            session.getTrackCount()
        );

    if (count <= 0)
        return;

    constexpr int gap = 8;

    const auto stripWidth =
        juce::jmax (
            62,
            (
                body.getWidth()
                - gap
                    * juce::jmax (
                        0,
                        count - 1
                    )
            )
            / juce::jmax (
                1,
                count
            )
        );

    for (
        int index = 0;
        index < count;
        ++index
    )
    {
        if (
            body.getWidth()
            < stripWidth
        )
        {
            break;
        }

        auto strip =
            body.removeFromLeft (
                stripWidth
            );

        body.removeFromLeft (gap);

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
            colour.withAlpha (0.15f)
        );

        g.fillRoundedRectangle (
            strip.toFloat(),
            8.0f
        );

        g.setColour (
            colour.withAlpha (0.45f)
        );

        g.drawRoundedRectangle (
            strip.toFloat(),
            8.0f,
            1.0f
        );

        auto text =
            strip.reduced (8);

        g.setColour (
            juce::Colours::white
                .withAlpha (0.86f)
        );

        g.setFont (
            juce::FontOptions (10.0f)
                .withStyle ("Bold")
        );

        g.drawText (
            juce::String (
                track->getName()
            ),
            text.removeFromTop (24),
            juce::Justification::centred
        );

        auto meter =
            text.reduced (
                text.getWidth() / 3,
                12
            );

        g.setColour (
            juce::Colours::black
                .withAlpha (0.45f)
        );

        g.fillRoundedRectangle (
            meter.toFloat(),
            4.0f
        );

        const auto gain =
            juce::jlimit (
                0.0f,
                1.0f,
                track->getGain()
                    * 0.5f
            );

        auto fill =
            meter;

        fill.removeFromTop (
            static_cast<int> (
                fill.getHeight()
                * (1.0f - gain)
            )
        );

        g.setColour (
            colour.withAlpha (0.86f)
        );

        g.fillRoundedRectangle (
            fill.toFloat(),
            4.0f
        );
    }
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

    g.drawText (
        "JAD",
        header.reduced (18),
        juce::Justification::centredLeft
    );

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
    // EXP-050E.2D AUDIO COCKPIT GEOMETRY
    audioSettingsButton.setBounds (
        getWidth() - 150,
        12,
        130,
        30
    );

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

    statusLabel.setBounds (
        commandBar
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

    const int dockHeight =
        juce::jlimit (
            210,
            310,
            area.getHeight() / 3
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

            controls.removeFromTop (25);

            auto toggleLine =
                controls.removeFromTop (17);

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






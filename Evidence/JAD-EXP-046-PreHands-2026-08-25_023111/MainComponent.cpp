#include "MainComponent.h"

#include <algorithm>

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
      keyboardPanel (
          context.getAudioSystem(),
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
        "PROJECT • LIVE",
        juce::dontSendNotification
    );

    statusLabel.setFont (
        juce::FontOptions (11.0f)
    );

    statusLabel.setJustificationType (
        juce::Justification::centredRight
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
        keyboardTabButton
    );

    addAndMakeVisible (
        synthTabButton
    );

    addAndMakeVisible (
        mixerTabButton
    );

    sessionOverviewPanel.setVisible (
        false
    );

    setDockPage (
        DockPage::keyboard
    );

    startTimerHz (20);
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
    refreshWorkspace();
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
    {
        selectedTrackIndex = -1;
    }

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

    statusLabel.setText (
        "TRACKS "
            + juce::String (count)
            + "  •  AUDIO "
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
        0.0,
        4.0
    );

    selectedTrackIndex =
        static_cast<int> (
            session.getTrackCount()
        ) - 1;

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

    session.removeTrack (
        track->getId()
    );

    const auto count =
        static_cast<int> (
            session.getTrackCount()
        );

    if (count <= 0)
    {
        selectedTrackIndex = -1;
    }

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

    repaint();
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
        accent.withAlpha (alpha + 0.08f),
        bounds.getTopLeft(),
        jadLookAndFeel.colourB()
            .withAlpha (alpha * 0.45f),
        bounds.getBottomRight(),
        false
    );

    g.setGradientFill (glass);

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

    auto body =
        area.reduced (8);

    body.removeFromTop (30);

    auto& session =
        context.getSessionState();

    const auto count =
        static_cast<int> (
            session.getTrackCount()
        );

    if (count <= 0)
    {
        g.setColour (
            juce::Colours::white
                .withAlpha (0.42f)
        );

        g.setFont (12.0f);

        g.drawText (
            "NO TRACKS",
            body,
            juce::Justification::centred
        );

        return;
    }

    const int rowHeight = 58;
    const int gap = 5;

    for (
        int index = 0;
        index < count;
        ++index
    )
    {
        auto* track =
            session.getTrack (
                static_cast<std::size_t> (
                    index
                )
            );

        if (track == nullptr)
            continue;

        if (
            body.getHeight()
            < rowHeight
        )
        {
            break;
        }

        auto row =
            body.removeFromTop (
                rowHeight
            );

        body.removeFromTop (gap);

        const auto colour =
            jadLookAndFeel.trackColour (
                index
            );

        const bool selected =
            index == selectedTrackIndex;

        juce::ColourGradient rowGradient (
            colour.withAlpha (
                selected ? 0.72f : 0.30f
            ),
            row.getTopLeft().toFloat(),
            jadLookAndFeel.colourB()
                .withAlpha (
                    selected ? 0.34f : 0.12f
                ),
            row.getBottomRight().toFloat(),
            false
        );

        g.setGradientFill (
            rowGradient
        );

        g.fillRoundedRectangle (
            row.toFloat(),
            9.0f
        );

        g.setColour (
            colour.withAlpha (
                selected ? 0.95f : 0.40f
            )
        );

        g.drawRoundedRectangle (
            row.toFloat(),
            9.0f,
            selected ? 2.0f : 1.0f
        );

        auto text =
            row.reduced (11, 5);

        g.setColour (
            juce::Colours::white
        );

        g.setFont (
            juce::FontOptions (13.0f)
                .withStyle (
                    selected
                        ? "Bold"
                        : "Regular"
                )
        );

        g.drawText (
            juce::String (
                track->getName()
            ),
            text.removeFromTop (24),
            juce::Justification::centredLeft
        );

        juce::String flags;

        if (track->isMuted())
            flags += "M ";

        if (track->isSolo())
            flags += "S ";

        if (track->isRecordArmed())
            flags += "R ";

        flags +=
            juce::String (
                static_cast<int> (
                    track->getClipCount()
                )
            )
            + " CLIP";

        if (track->getClipCount() != 1)
            flags += "S";

        g.setColour (
            juce::Colours::white
                .withAlpha (0.55f)
        );

        g.setFont (9.5f);

        g.drawText (
            flags,
            text,
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
        "ARRANGEMENT",
        jadLookAndFeel.colourD(),
        0.11f
    );

    auto body =
        area.reduced (8);

    body.removeFromTop (28);

    const int rulerHeight = 30;

    auto ruler =
        body.removeFromTop (
            rulerHeight
        );

    const int labelWidth = 4;

    auto timeline =
        body;

    const auto beatToX =
        [&timeline, this] (
            double beat)
        {
            return timeline.getX()
                + static_cast<int> (
                    (
                        beat
                        / visibleBeats
                    )
                    * timeline.getWidth()
                );
        };

    g.setColour (
        juce::Colours::black
            .withAlpha (0.28f)
    );

    g.fillRoundedRectangle (
        ruler.toFloat(),
        7.0f
    );

    for (
        int beat = 0;
        beat <= static_cast<int> (
            visibleBeats
        );
        ++beat
    )
    {
        const auto x =
            ruler.getX()
            + static_cast<int> (
                (
                    static_cast<double> (beat)
                    / visibleBeats
                )
                * ruler.getWidth()
            );

        const bool bar =
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
            x + labelWidth,
            ruler.getY(),
            36,
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

    if (count <= 0)
        return;

    const int laneGap = 5;

    const auto availableHeight =
        timeline.getHeight()
        - (
            laneGap
            * juce::jmax (
                0,
                count - 1
            )
        );

    const auto laneHeight =
        juce::jmax (
            28,
            availableHeight
                / juce::jmax (
                    1,
                    count
                )
        );

    auto lanes =
        timeline;

    for (
        int trackIndex = 0;
        trackIndex < count;
        ++trackIndex
    )
    {
        if (
            lanes.getHeight()
            < laneHeight
        )
        {
            break;
        }

        auto lane =
            lanes.removeFromTop (
                laneHeight
            );

        lanes.removeFromTop (
            laneGap
        );

        auto* track =
            session.getTrack (
                static_cast<std::size_t> (
                    trackIndex
                )
            );

        if (track == nullptr)
            continue;

        const auto colour =
            jadLookAndFeel.trackColour (
                trackIndex
            );

        const bool selected =
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

        for (
            std::size_t clipIndex = 0;
            clipIndex
                < track->getClipCount();
            ++clipIndex
        )
        {
            auto* clip =
                track->getClip (
                    clipIndex
                );

            if (clip == nullptr)
                continue;

            const auto start =
                juce::jlimit (
                    0.0,
                    visibleBeats,
                    clip->getStartBeat()
                );

            const auto end =
                juce::jlimit (
                    0.0,
                    visibleBeats,
                    clip->getStartBeat()
                        + clip->getLengthBeats()
                );

            if (end <= start)
                continue;

            const auto x1 =
                beatToX (start);

            const auto x2 =
                beatToX (end);

            juce::Rectangle<int> clipBounds (
                x1 + 3,
                lane.getY() + 5,
                juce::jmax (
                    8,
                    x2 - x1 - 6
                ),
                lane.getHeight() - 10
            );

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

            g.setColour (
                juce::Colours::white
                    .withAlpha (
                        clip->isMuted()
                            ? 0.42f
                            : 0.88f
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
            .getPositionBeats();

    const auto playheadX =
        beatToX (
            std::fmod (
                juce::jmax (
                    0.0,
                    playheadBeat
                ),
                visibleBeats
            )
        );

    g.setColour (
        juce::Colours::white
            .withAlpha (0.92f)
    );

    g.drawVerticalLine (
        playheadX,
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
            playheadX - 5
        ),
        static_cast<float> (
            ruler.getY()
        ),
        static_cast<float> (
            playheadX + 5
        ),
        static_cast<float> (
            ruler.getY()
        ),
        static_cast<float> (
            playheadX
        ),
        static_cast<float> (
            ruler.getY() + 8
        )
    );

    g.fillPath (head);
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

    body.removeFromTop (14);

    g.setColour (
        juce::Colours::white
    );

    g.setFont (
        juce::FontOptions (17.0f)
            .withStyle ("Bold")
    );

    g.drawText (
        juce::String (
            track->getName()
        ),
        body.removeFromTop (32),
        juce::Justification::centredLeft
    );

    g.setFont (10.5f);

    g.setColour (
        juce::Colours::white
            .withAlpha (0.58f)
    );

    g.drawText (
        "TRACK "
            + juce::String (
                selectedTrackIndex + 1
            ),
        body.removeFromTop (22),
        juce::Justification::centredLeft
    );

    body.removeFromTop (12);

    const auto drawValue =
        [&g, &body, colour] (
            const juce::String& label,
            const juce::String& value)
        {
            auto row =
                body.removeFromTop (34);

            g.setColour (
                juce::Colours::white
                    .withAlpha (0.42f)
            );

            g.setFont (9.5f);

            g.drawText (
                label,
                row.removeFromTop (14),
                juce::Justification::centredLeft
            );

            g.setColour (
                colour.withAlpha (0.94f)
            );

            g.setFont (
                juce::FontOptions (13.0f)
                    .withStyle ("Bold")
            );

            g.drawText (
                value,
                row,
                juce::Justification::centredLeft
            );

            body.removeFromTop (7);
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

    drawValue (
        "CLIPS",
        juce::String (
            static_cast<int> (
                track->getClipCount()
            )
        )
    );

    juce::String state;

    state +=
        track->isMuted()
            ? "MUTE "
            : "";

    state +=
        track->isSolo()
            ? "SOLO "
            : "";

    state +=
        track->isRecordArmed()
            ? "ARM "
            : "";

    if (state.isEmpty())
        state = "READY";

    drawValue (
        "STATE",
        state.trim()
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

    const int gap = 8;

    const auto stripWidth =
        juce::jmax (
            62,
            (
                body.getWidth()
                - (
                    gap
                    * juce::jmax (
                        0,
                        count - 1
                    )
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

        const auto meterArea =
            text.reduced (
                text.getWidth() / 3,
                12
            );

        g.setColour (
            juce::Colours::black
                .withAlpha (0.45f)
        );

        g.fillRoundedRectangle (
            meterArea.toFloat(),
            4.0f
        );

        const auto gain =
            juce::jlimit (
                0.0f,
                1.0f,
                track->getGain() * 0.5f
            );

        auto fill =
            meterArea;

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

juce::Rectangle<int>
MainComponent::getTrackRackBounds() const
{
    return cachedTrackRack;
}

juce::Rectangle<int>
MainComponent::getArrangementBounds() const
{
    return cachedArrangement;
}

juce::Rectangle<int>
MainComponent::getInspectorBounds() const
{
    return cachedInspector;
}

juce::Rectangle<int>
MainComponent::getDockBounds() const
{
    return cachedDock;
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

    auto aura =
        bounds.reduced (10.0f);

    g.setColour (
        jadLookAndFeel.colourD()
            .withAlpha (0.07f)
    );

    g.fillRoundedRectangle (
        aura,
        30.0f
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

    g.setColour (
        juce::Colours::white
            .withAlpha (0.72f)
    );

    g.drawText (
        "JUST ANOTHER DAW  •  WORKSPACE CHASSIS",
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
    auto area =
        getLocalBounds();

    area.removeFromTop (68);

    auto commandBar =
        area.removeFromTop (46)
            .reduced (18, 3);

    projectLabel.setBounds (
        commandBar.removeFromLeft (
            juce::jmin (
                280,
                commandBar.getWidth() / 3
            )
        )
    );

    commandBar.removeFromLeft (8);

    addTrackButton.setBounds (
        commandBar.removeFromLeft (94)
            .reduced (2)
    );

    deleteTrackButton.setBounds (
        commandBar.removeFromLeft (94)
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
            180,
            250,
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

    dockControls.removeFromTop (4);

    keyboardPanel.setBounds (
        dockControls
    );

    synthVoicePanel.setBounds (
        dockControls
    );

    sessionOverviewPanel.setBounds (
        {}
    );
}

void MainComponent::mouseDown (
    const juce::MouseEvent& event)
{
    if (
        ! cachedTrackRack.contains (
            event.getPosition()
        )
    )
    {
        return;
    }

    auto body =
        cachedTrackRack.reduced (8);

    body.removeFromTop (30);

    const int rowHeight = 58;
    const int gap = 5;

    auto& session =
        context.getSessionState();

    const auto count =
        static_cast<int> (
            session.getTrackCount()
        );

    for (
        int index = 0;
        index < count;
        ++index
    )
    {
        if (
            body.getHeight()
            < rowHeight
        )
        {
            break;
        }

        auto row =
            body.removeFromTop (
                rowHeight
            );

        body.removeFromTop (
            gap
        );

        if (
            row.contains (
                event.getPosition()
            )
        )
        {
            selectTrack (
                index
            );

            return;
        }
    }
}

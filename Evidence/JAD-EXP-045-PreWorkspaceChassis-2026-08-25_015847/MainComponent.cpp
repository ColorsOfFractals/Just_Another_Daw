#include "MainComponent.h"

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
        1440,
        960
    );

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
}

MainComponent::~MainComponent()
{
    setLookAndFeel (
        nullptr
    );
}

void MainComponent::drawTrackLane (
    juce::Graphics& g,
    juce::Rectangle<int> lane,
    int trackIndex,
    const juce::String& name)
{
    const auto colour =
        jadLookAndFeel.trackColour (
            trackIndex
        );

    auto header =
        lane.removeFromLeft (138);

    juce::ColourGradient headerGradient (
        colour.withAlpha (0.66f),
        header.getTopLeft().toFloat(),
        colour
            .interpolatedWith (
                juce::Colours::black,
                0.55f
            )
            .withAlpha (0.54f),
        header.getBottomRight().toFloat(),
        false
    );

    g.setGradientFill (
        headerGradient
    );

    g.fillRoundedRectangle (
        header.toFloat(),
        10.0f
    );

    g.setColour (
        juce::Colours::white
            .withAlpha (0.94f)
    );

    g.setFont (
        juce::FontOptions (14.0f)
            .withStyle ("Bold")
    );

    g.drawText (
        name,
        header.reduced (12),
        juce::Justification::centredLeft
    );

    lane.removeFromLeft (8);

    juce::ColourGradient laneGradient (
        colour.withAlpha (0.19f),
        lane.getTopLeft().toFloat(),
        jadLookAndFeel.colourB()
            .withAlpha (0.06f),
        lane.getBottomRight().toFloat(),
        false
    );

    g.setGradientFill (
        laneGradient
    );

    g.fillRoundedRectangle (
        lane.toFloat(),
        10.0f
    );

    g.setColour (
        colour.withAlpha (0.42f)
    );

    g.drawRoundedRectangle (
        lane.toFloat(),
        10.0f,
        1.0f
    );

    const auto clipWidth =
        juce::jmax (
            80,
            lane.getWidth() / 5
        );

    auto clip =
        lane.reduced (8)
            .removeFromLeft (
                clipWidth
            );

    juce::ColourGradient clipGradient (
        colour.withAlpha (0.82f),
        clip.getTopLeft().toFloat(),
        jadLookAndFeel.colourD()
            .withAlpha (0.48f),
        clip.getBottomRight().toFloat(),
        false
    );

    g.setGradientFill (
        clipGradient
    );

    g.fillRoundedRectangle (
        clip.toFloat(),
        8.0f
    );

    g.setColour (
        juce::Colours::white
            .withAlpha (0.78f)
    );

    g.setFont (11.0f);

    g.drawText (
        "CLIP TERRITORY",
        clip.reduced (8),
        juce::Justification::centredLeft
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
            7,
            5,
            18
        )
    );

    juce::ColourGradient background (
        jadLookAndFeel.colourA()
            .withAlpha (0.33f),
        bounds.getTopLeft(),
        jadLookAndFeel.colourC()
            .withAlpha (0.22f),
        bounds.getBottomRight(),
        false
    );

    background.addColour (
        0.48,
        jadLookAndFeel.colourB()
            .withAlpha (0.26f)
    );

    background.addColour (
        0.76,
        jadLookAndFeel.colourD()
            .withAlpha (0.20f)
    );

    g.setGradientFill (
        background
    );

    g.fillRect (
        bounds
    );

    auto aura =
        bounds.reduced (12.0f);

    g.setColour (
        jadLookAndFeel.colourD()
            .withAlpha (0.12f)
    );

    g.fillRoundedRectangle (
        aura,
        28.0f
    );

    auto header =
        getLocalBounds()
            .removeFromTop (72)
            .reduced (24, 8);

    juce::ColourGradient headerGradient (
        jadLookAndFeel.colourA()
            .withAlpha (0.80f),
        header.getTopLeft().toFloat(),
        jadLookAndFeel.colourD()
            .withAlpha (0.55f),
        header.getBottomRight().toFloat(),
        false
    );

    g.setGradientFill (
        headerGradient
    );

    g.fillRoundedRectangle (
        header.toFloat(),
        18.0f
    );

    g.setColour (
        juce::Colours::white
    );

    g.setFont (
        juce::FontOptions (27.0f)
            .withStyle ("Bold")
    );

    g.drawText (
        "JAD",
        header.reduced (22),
        juce::Justification::centredLeft
    );

    g.setFont (12.0f);

    g.drawText (
        "JUST ANOTHER DAW  •  CHROMATIC CITY  •  AUDIO HIGHWAY ONLINE",
        header.reduced (22),
        juce::Justification::centredRight
    );

    auto world =
        getLocalBounds();

    world.removeFromTop (178);
    world.reduce (24, 12);

    auto upper =
        world.removeFromTop (
            juce::jmax (
                240,
                world.getHeight() / 2
            )
        );

    auto arrangement =
        upper.removeFromLeft (
            static_cast<int> (
                upper.getWidth() * 0.64
            )
        );

    arrangement.reduce (
        10,
        8
    );

    auto ruler =
        arrangement.removeFromTop (26);

    g.setColour (
        juce::Colours::white
            .withAlpha (0.58f)
    );

    g.setFont (10.0f);

    for (
        int beat = 0;
        beat < 9;
        ++beat
    )
    {
        const auto x =
            ruler.getX()
            + (
                beat
                * ruler.getWidth()
                / 8
            );

        g.drawText (
            juce::String (beat + 1),
            x,
            ruler.getY(),
            30,
            ruler.getHeight(),
            juce::Justification::centredLeft
        );

        g.setColour (
            jadLookAndFeel.colourD()
                .withAlpha (0.18f)
        );

        g.drawVerticalLine (
            x,
            static_cast<float> (
                arrangement.getY()
            ),
            static_cast<float> (
                arrangement.getBottom()
            )
        );

        g.setColour (
            juce::Colours::white
                .withAlpha (0.58f)
        );
    }

    const int gap = 7;

    const auto laneHeight =
        (
            arrangement.getHeight()
            - (gap * 3)
        ) / 4;

    auto lane1 =
        arrangement.removeFromTop (
            laneHeight
        );

    arrangement.removeFromTop (gap);

    auto lane2 =
        arrangement.removeFromTop (
            laneHeight
        );

    arrangement.removeFromTop (gap);

    auto lane3 =
        arrangement.removeFromTop (
            laneHeight
        );

    arrangement.removeFromTop (gap);

    auto lane4 =
        arrangement;

    drawTrackLane (
        g,
        lane1,
        0,
        "TRACK 1 • SYNTH"
    );

    drawTrackLane (
        g,
        lane2,
        1,
        "TRACK 2"
    );

    drawTrackLane (
        g,
        lane3,
        2,
        "TRACK 3"
    );

    drawTrackLane (
        g,
        lane4,
        3,
        "TRACK 4"
    );
}

void MainComponent::resized()
{
    auto area =
        getLocalBounds();

    area.removeFromTop (76);

    auto transportArea =
        area.removeFromTop (92)
            .reduced (24, 8);

    transportPanel.setBounds (
        transportArea
    );

    area.reduce (
        24,
        12
    );

    auto upper =
        area.removeFromTop (
            juce::jmax (
                240,
                area.getHeight() / 2
            )
        );

    auto arrangementTerritory =
        upper.removeFromLeft (
            static_cast<int> (
                upper.getWidth() * 0.64
            )
        );

    juce::ignoreUnused (
        arrangementTerritory
    );

    upper.removeFromLeft (14);

    auto rightTop =
        upper.removeFromTop (
            upper.getHeight() / 2
        );

    sessionOverviewPanel.setBounds (
        rightTop.reduced (5)
    );

    synthVoicePanel.setBounds (
        upper.reduced (5)
    );

    area.removeFromTop (12);

    keyboardPanel.setBounds (
        area.reduced (4)
    );
}

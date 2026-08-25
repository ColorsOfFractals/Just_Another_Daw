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
      )
{
    setSize (
        1260,
        800
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
}

void MainComponent::paint (
    juce::Graphics& g)
{
    const auto bounds =
        getLocalBounds().toFloat();

    // ------------------------------------------------------------
    // FIRST JAD SHELL COLOUR LANGUAGE
    // BLACK -> DEEP RED -> BLACK
    // ------------------------------------------------------------

    juce::ColourGradient background (
        juce::Colour::fromRGB (
            3,
            3,
            5
        ),
        0.0f,
        0.0f,

        juce::Colour::fromRGB (
            48,
            0,
            7
        ),
        bounds.getCentreX(),
        bounds.getHeight()
            * 0.42f,

        false
    );

    background.addColour (
        0.62,
        juce::Colour::fromRGB (
            18,
            0,
            4
        )
    );

    background.addColour (
        1.0,
        juce::Colour::fromRGB (
            2,
            2,
            4
        )
    );

    g.setGradientFill (
        background
    );

    g.fillAll();

    // ------------------------------------------------------------
    // HEADER
    // ------------------------------------------------------------

    auto header =
        getLocalBounds()
            .removeFromTop (72);

    g.setColour (
        juce::Colour::fromRGB (
            5,
            5,
            7
        )
        .withAlpha (0.82f)
    );

    g.fillRect (
        header
    );

    g.setColour (
        juce::Colour::fromRGB (
            210,
            25,
            35
        )
    );

    g.drawHorizontalLine (
        header.getBottom() - 1,
        0.0f,
        static_cast<float> (
            getWidth()
        )
    );

    g.setColour (
        juce::Colours::white
    );

    g.setFont (30.0f);

    g.drawText (
        "JAD",
        24,
        0,
        120,
        header.getHeight(),
        juce::Justification::centredLeft
    );

    auto& audio =
        context.getAudioSystem();

    g.setFont (13.0f);

    g.setColour (
        audio.isReady()
            ? juce::Colour::fromRGB (
                255,
                85,
                90
            )
            : juce::Colours::orange
    );

    g.drawText (
        audio.isReady()
            ? "AUDIO ONLINE"
            : "AUDIO OFFLINE",
        getWidth() - 180,
        0,
        150,
        header.getHeight(),
        juce::Justification::centredRight
    );

    // ------------------------------------------------------------
    // UNDER-CONSTRUCTION ARRANGEMENT TERRITORY
    // ------------------------------------------------------------

    auto world =
        getLocalBounds();

    world.removeFromTop (160);
    world.reduce (24, 20);

    auto arrangementHint =
        world.removeFromTop (30);

    g.setColour (
        juce::Colour::fromRGB (
            160,
            50,
            60
        )
    );

    g.setFont (12.0f);

    g.drawText (
        "SESSION / ARRANGEMENT CONSTRUCTION",
        arrangementHint,
        juce::Justification::centredLeft
    );
}

void MainComponent::resized()
{
    auto area =
        getLocalBounds();

    area.removeFromTop (72);

    auto transportArea =
        area.removeFromTop (78)
            .reduced (24, 10);

    transportPanel.setBounds (
        transportArea
    );

    area.reduce (
        24,
        24
    );

    area.removeFromTop (28);

    const int gap =
        22;

    auto left =
        area.removeFromLeft (
            (area.getWidth() - gap) / 2
        );

    area.removeFromLeft (
        gap
    );

    sessionOverviewPanel.setBounds (
        left
    );

    synthVoicePanel.setBounds (
        area
    );
}

#include "MainComponent.h"

MainComponent::MainComponent (
    JADContext& contextToUse)
    : context (contextToUse),
      sessionOverviewPanel (
          context.getSessionState()
      ),
      synthVoicePanel (
          context.getAudioSystem()
      )
{
    setSize (
        1180,
        760
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
    g.fillAll (
        juce::Colour::fromRGB (
            5,
            8,
            15
        )
    );

    auto top =
        getLocalBounds()
            .removeFromTop (94)
            .reduced (24, 12);

    g.setColour (
        juce::Colours::white
    );

    g.setFont (32.0f);

    g.drawText (
        "JAD",
        top.removeFromTop (42),
        juce::Justification::centred
    );

    auto& audio =
        context.getAudioSystem();

    g.setFont (14.0f);

    g.setColour (
        audio.isReady()
            ? juce::Colour::fromRGB (
                100,
                255,
                145
            )
            : juce::Colours::orange
    );

    g.drawText (
        audio.isReady()
            ? "AUDIO SYSTEM ONLINE"
            : "AUDIO SYSTEM OFFLINE",
        top.removeFromTop (24),
        juce::Justification::centred
    );

    g.setColour (
        juce::Colour::fromRGB (
            35,
            110,
            135
        )
    );

    g.drawHorizontalLine (
        94,
        20.0f,
        static_cast<float> (
            getWidth() - 20
        )
    );
}

void MainComponent::resized()
{
    auto world =
        getLocalBounds();

    world.removeFromTop (112);

    world.reduce (
        26,
        22
    );

    const int gap =
        22;

    auto left =
        world.removeFromLeft (
            (world.getWidth() - gap) / 2
        );

    world.removeFromLeft (
        gap
    );

    auto right =
        world;

    sessionOverviewPanel.setBounds (
        left
    );

    synthVoicePanel.setBounds (
        right
    );
}

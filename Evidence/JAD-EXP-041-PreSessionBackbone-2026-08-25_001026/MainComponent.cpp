#include "MainComponent.h"

MainComponent::MainComponent (JADContext& contextToUse)
    : context (contextToUse),
      synthVoicePanel (context.getAudioSystem())
{
    setSize (800, 680);

    addAndMakeVisible (
        synthVoicePanel
    );
}

void MainComponent::paint (juce::Graphics& g)
{
    g.fillAll (
        juce::Colour::fromRGB (
            8,
            10,
            18
        )
    );

    auto area =
        getLocalBounds().reduced (32);

    auto header =
        area.removeFromTop (180);

    auto& audio =
        context.getAudioSystem();

    g.setColour (
        juce::Colours::white
    );

    g.setFont (34.0f);

    g.drawText (
        "JAD",
        header.removeFromTop (48),
        juce::Justification::centred
    );

    g.setFont (17.0f);

    g.setColour (
        audio.isReady()
            ? juce::Colours::lightgreen
            : juce::Colours::orange
    );

    g.drawText (
        audio.isReady()
            ? "AUDIO SYSTEM ONLINE"
            : "AUDIO SYSTEM UNAVAILABLE",
        header.removeFromTop (30),
        juce::Justification::centred
    );

    g.setColour (
        juce::Colours::lightgrey
    );

    g.setFont (14.0f);

    g.drawText (
        "Output : "
            + audio.getOutputDeviceName(),
        header.removeFromTop (24),
        juce::Justification::centred
    );

    g.drawText (
        "Rate   : "
            + juce::String (
                audio.getSampleRate(),
                0
            )
            + " Hz",
        header.removeFromTop (24),
        juce::Justification::centred
    );

    g.drawText (
        "Buffer : "
            + juce::String (
                audio.getBufferSize()
            )
            + " samples",
        header.removeFromTop (24),
        juce::Justification::centred
    );
}

void MainComponent::resized()
{
    auto area =
        getLocalBounds().reduced (42);

    area.removeFromTop (190);

    synthVoicePanel.setBounds (
        area.withSizeKeepingCentre (
            juce::jmin (
                650,
                area.getWidth()
            ),
            juce::jmin (
                400,
                area.getHeight()
            )
        )
    );
}

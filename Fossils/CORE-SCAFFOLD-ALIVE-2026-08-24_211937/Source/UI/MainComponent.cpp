#include "MainComponent.h"

MainComponent::MainComponent (JADContext& contextToUse)
    : context (contextToUse)
{
    setSize (720, 420);
}

void MainComponent::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colour::fromRGB (18, 18, 24));

    g.setColour (juce::Colours::white);
    g.setFont (32.0f);

    g.drawFittedText (
        "JAD",
        getLocalBounds().reduced (20),
        juce::Justification::centred,
        1
    );
}

void MainComponent::resized()
{
}

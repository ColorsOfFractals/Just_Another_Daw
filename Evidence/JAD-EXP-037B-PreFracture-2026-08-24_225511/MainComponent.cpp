#include "MainComponent.h"

MainComponent::MainComponent (JADContext& contextToUse)
    : context (contextToUse)
{
    setSize (800, 680);

    addAndMakeVisible (voiceButton);
    addAndMakeVisible (frequencySlider);
    addAndMakeVisible (gainSlider);
    addAndMakeVisible (waveformBox);

    addAndMakeVisible (frequencyLabel);
    addAndMakeVisible (gainLabel);
    addAndMakeVisible (waveformLabel);

    voiceButton.setClickingTogglesState (true);

    frequencyLabel.setText (
        "FREQUENCY",
        juce::dontSendNotification
    );

    gainLabel.setText (
        "GAIN",
        juce::dontSendNotification
    );

    waveformLabel.setText (
        "WAVEFORM",
        juce::dontSendNotification
    );

    for (auto* label : {
        &frequencyLabel,
        &gainLabel,
        &waveformLabel
    })
    {
        label->setJustificationType (
            juce::Justification::centred
        );
    }

    frequencySlider.setRange (
        40.0,
        2000.0,
        1.0
    );

    frequencySlider.setValue (440.0);
    frequencySlider.setTextValueSuffix (" Hz");
    frequencySlider.setSkewFactorFromMidPoint (440.0);

    gainSlider.setRange (
        0.0,
        0.20,
        0.001
    );

    gainSlider.setValue (0.05);
    gainSlider.setTextValueSuffix (" gain");

    waveformBox.addItem ("SINE", 1);
    waveformBox.addItem ("SAW", 2);
    waveformBox.addItem ("SQUARE", 3);
    waveformBox.setSelectedId (1);

    voiceButton.onClick = [this]
    {
        context.getAudioSystem().setTestToneEnabled (
            voiceButton.getToggleState()
        );

        repaint();
    };

    frequencySlider.onValueChange = [this]
    {
        context.getAudioSystem().setFrequency (
            frequencySlider.getValue()
        );

        repaint();
    };

    gainSlider.onValueChange = [this]
    {
        context.getAudioSystem().setGain (
            static_cast<float> (
                gainSlider.getValue()
            )
        );

        repaint();
    };

    waveformBox.onChange = [this]
    {
        const auto id =
            waveformBox.getSelectedId();

        if (id == 1)
        {
            context.getAudioSystem().setWaveform (
                AudioSystem::Waveform::sine
            );
        }

        if (id == 2)
        {
            context.getAudioSystem().setWaveform (
                AudioSystem::Waveform::saw
            );
        }

        if (id == 3)
        {
            context.getAudioSystem().setWaveform (
                AudioSystem::Waveform::square
            );
        }

        repaint();
    };
}

MainComponent::~MainComponent()
{
    context.getAudioSystem().setTestToneEnabled (false);
}

void MainComponent::paint (juce::Graphics& g)
{
    g.fillAll (
        juce::Colour::fromRGB (8, 10, 18)
    );

    auto bounds =
        getLocalBounds().reduced (40);

    auto& audio =
        context.getAudioSystem();

    g.setColour (juce::Colours::white);
    g.setFont (36.0f);

    g.drawText (
        "JAD",
        bounds.removeFromTop (55),
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
        bounds.removeFromTop (35),
        juce::Justification::centred
    );

    bounds.removeFromTop (10);

    g.setColour (juce::Colours::lightgrey);
    g.setFont (15.0f);

    const juce::StringArray lines
    {
        "Output : " + audio.getOutputDeviceName(),

        "Rate   : "
            + juce::String (
                audio.getSampleRate(),
                0
            )
            + " Hz",

        "Buffer : "
            + juce::String (
                audio.getBufferSize()
            )
            + " samples"
    };

    for (const auto& line : lines)
    {
        g.drawText (
            line,
            bounds.removeFromTop (26),
            juce::Justification::centred
        );
    }

    bounds.removeFromTop (12);

    g.setFont (18.0f);

    g.setColour (
        audio.isTestToneEnabled()
            ? juce::Colours::lightgreen
            : juce::Colours::grey
    );

    g.drawText (
        audio.isTestToneEnabled()
            ? "VOICE ACTIVE"
            : "VOICE SILENT",
        bounds.removeFromTop (32),
        juce::Justification::centred
    );
}

void MainComponent::resized()
{
    const int centreX = getWidth() / 2;
    const int controlWidth = 620;

    waveformLabel.setBounds (
        centreX - controlWidth / 2,
        390,
        controlWidth,
        22
    );

    waveformBox.setBounds (
        centreX - controlWidth / 2,
        414,
        controlWidth,
        36
    );

    frequencyLabel.setBounds (
        centreX - controlWidth / 2,
        465,
        controlWidth,
        22
    );

    frequencySlider.setBounds (
        centreX - controlWidth / 2,
        489,
        controlWidth,
        40
    );

    gainLabel.setBounds (
        centreX - controlWidth / 2,
        540,
        controlWidth,
        22
    );

    gainSlider.setBounds (
        centreX - controlWidth / 2,
        564,
        controlWidth,
        40
    );

    voiceButton.setBounds (
        centreX - 110,
        620,
        220,
        42
    );
}

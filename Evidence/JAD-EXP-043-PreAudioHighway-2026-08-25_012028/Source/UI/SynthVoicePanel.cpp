#include "SynthVoicePanel.h"

SynthVoicePanel::SynthVoicePanel (AudioSystem& audioSystem)
    : audio (audioSystem)
{
    titleLabel.setText (
        "SYNTH VOICE",
        juce::dontSendNotification
    );

    titleLabel.setJustificationType (
        juce::Justification::centred
    );

    addAndMakeVisible (titleLabel);

    waveformLabel.setText (
        "WAVEFORM",
        juce::dontSendNotification
    );

    frequencyLabel.setText (
        "FREQUENCY",
        juce::dontSendNotification
    );

    gainLabel.setText (
        "GAIN",
        juce::dontSendNotification
    );

    for (auto* label : {
        &waveformLabel,
        &frequencyLabel,
        &gainLabel
    })
    {
        label->setJustificationType (
            juce::Justification::centred
        );

        addAndMakeVisible (*label);
    }

    waveformBox.addItem ("SINE",   1);
    waveformBox.addItem ("SAW",    2);
    waveformBox.addItem ("SQUARE", 3);

    const auto waveform =
        audio.getWaveform();

    if (waveform == AudioSystem::Waveform::sine)
        waveformBox.setSelectedId (1, juce::dontSendNotification);

    if (waveform == AudioSystem::Waveform::saw)
        waveformBox.setSelectedId (2, juce::dontSendNotification);

    if (waveform == AudioSystem::Waveform::square)
        waveformBox.setSelectedId (3, juce::dontSendNotification);

    waveformBox.onChange = [this]
    {
        const auto id =
            waveformBox.getSelectedId();

        if (id == 1)
            audio.setWaveform (AudioSystem::Waveform::sine);

        if (id == 2)
            audio.setWaveform (AudioSystem::Waveform::saw);

        if (id == 3)
            audio.setWaveform (AudioSystem::Waveform::square);
    };

    addAndMakeVisible (waveformBox);

    frequencySlider.setRange (
        40.0,
        2000.0,
        1.0
    );

    frequencySlider.setValue (
        audio.getFrequency(),
        juce::dontSendNotification
    );

    frequencySlider.setTextValueSuffix (" Hz");

    frequencySlider.setSkewFactorFromMidPoint (
        440.0
    );

    frequencySlider.onValueChange = [this]
    {
        audio.setFrequency (
            frequencySlider.getValue()
        );
    };

    addAndMakeVisible (frequencySlider);

    gainSlider.setRange (
        0.0,
        0.20,
        0.001
    );

    gainSlider.setValue (
        audio.getGain(),
        juce::dontSendNotification
    );

    gainSlider.setTextValueSuffix (" gain");

    gainSlider.onValueChange = [this]
    {
        audio.setGain (
            static_cast<float> (
                gainSlider.getValue()
            )
        );
    };

    addAndMakeVisible (gainSlider);

    voiceButton.setClickingTogglesState (true);

    voiceButton.setToggleState (
        audio.isTestToneEnabled(),
        juce::dontSendNotification
    );

    voiceButton.onClick = [this]
    {
        audio.setTestToneEnabled (
            voiceButton.getToggleState()
        );

        repaint();
    };

    addAndMakeVisible (voiceButton);
}

SynthVoicePanel::~SynthVoicePanel()
{
    audio.setTestToneEnabled (false);
}

void SynthVoicePanel::paint (juce::Graphics& g)
{
    auto bounds =
        getLocalBounds()
            .toFloat()
            .reduced (1.0f);

    g.setColour (
        juce::Colour::fromRGB (
            12,
            16,
            24
        )
    );

    g.fillRoundedRectangle (
        bounds,
        12.0f
    );

    g.setColour (
        voiceButton.getToggleState()
            ? juce::Colour::fromRGB (95, 230, 145)
            : juce::Colour::fromRGB (65, 105, 125)
    );

    g.drawRoundedRectangle (
        bounds,
        12.0f,
        1.5f
    );
}

void SynthVoicePanel::resized()
{
    auto area =
        getLocalBounds().reduced (28);

    titleLabel.setBounds (
        area.removeFromTop (30)
    );

    area.removeFromTop (14);

    waveformLabel.setBounds (
        area.removeFromTop (22)
    );

    waveformBox.setBounds (
        area.removeFromTop (38)
    );

    area.removeFromTop (14);

    frequencyLabel.setBounds (
        area.removeFromTop (22)
    );

    frequencySlider.setBounds (
        area.removeFromTop (40)
    );

    area.removeFromTop (14);

    gainLabel.setBounds (
        area.removeFromTop (22)
    );

    gainSlider.setBounds (
        area.removeFromTop (40)
    );

    area.removeFromTop (20);

    voiceButton.setBounds (
        area.removeFromTop (44)
            .withSizeKeepingCentre (
                240,
                44
            )
    );
}

#include "AudioSettingsPanel.h"

AudioSettingsPanel::AudioSettingsPanel (
    AudioSystem& audioSystemToUse,
    JADLookAndFeel& lookAndFeelToUse)
    : audioSystem (audioSystemToUse),
      jadLookAndFeel (lookAndFeelToUse),
      deviceSelector (
          audioSystem.getDeviceManager(),
          0,
          2,
          0,
          2,
          true,
          false,
          true,
          false
      )
{
    titleLabel.setText (
        "AUDIO I/O",
        juce::dontSendNotification
    );

    titleLabel.setFont (
        juce::FontOptions (18.0f)
            .withStyle ("Bold")
    );

    statusLabel.setFont (
        juce::FontOptions (13.0f)
            .withStyle ("Bold")
    );

    detailLabel.setFont (
        juce::FontOptions (11.0f)
    );

    detailLabel.setJustificationType (
        juce::Justification::centredRight
    );

    closeButton.onClick =
        [this]
        {
            if (onClose)
                onClose();
        };

    addAndMakeVisible (titleLabel);
    addAndMakeVisible (statusLabel);
    addAndMakeVisible (detailLabel);
    addAndMakeVisible (closeButton);
    addAndMakeVisible (deviceSelector);

    refreshStatus();
    startTimerHz (4);
}

AudioSettingsPanel::~AudioSettingsPanel()
{
    stopTimer();
}

void AudioSettingsPanel::timerCallback()
{
    refreshStatus();
}

void AudioSettingsPanel::refreshStatus()
{
    auto* device =
        audioSystem.getDeviceManager()
            .getCurrentAudioDevice();

    if (device == nullptr)
    {
        statusLabel.setText (
            "AUDIO OFFLINE",
            juce::dontSendNotification
        );

        detailLabel.setText (
            "NO ACTIVE DEVICE",
            juce::dontSendNotification
        );

        return;
    }

    statusLabel.setText (
        "AUDIO ONLINE",
        juce::dontSendNotification
    );

    detailLabel.setText (
        juce::String (
            device->getCurrentSampleRate(),
            0
        )
            + " Hz  /  "
            + juce::String (
                device->getCurrentBufferSizeSamples()
            )
            + " samples",
        juce::dontSendNotification
    );
}

void AudioSettingsPanel::paint (
    juce::Graphics& g)
{
    const auto bounds =
        getLocalBounds().toFloat();

    g.setColour (
        juce::Colour::fromRGB (
            7,
            5,
            18
        )
            .withAlpha (0.98f)
    );

    g.fillRoundedRectangle (
        bounds,
        16.0f
    );

    g.setColour (
        jadLookAndFeel.colourA()
            .withAlpha (0.90f)
    );

    g.drawRoundedRectangle (
        bounds.reduced (1.0f),
        16.0f,
        2.0f
    );
}

void AudioSettingsPanel::resized()
{
    auto area =
        getLocalBounds()
            .reduced (18);

    auto header =
        area.removeFromTop (38);

    titleLabel.setBounds (
        header.removeFromLeft (180)
    );

    closeButton.setBounds (
        header.removeFromRight (42)
            .reduced (3)
    );

    detailLabel.setBounds (
        header.removeFromRight (230)
    );

    statusLabel.setBounds (
        header
    );

    area.removeFromTop (10);

    deviceSelector.setBounds (
        area
    );
}

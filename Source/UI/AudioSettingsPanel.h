#pragma once

#include <juce_audio_utils/juce_audio_utils.h>
#include <juce_gui_basics/juce_gui_basics.h>

#include "../Audio/AudioSystem.h"
#include "../Audio/MidiSystem.h"

#include "JADLookAndFeel.h"

class PreferencesTabButton final :
    public juce::TextButton
{
public:
    explicit PreferencesTabButton (
        const juce::String& buttonText)
        : juce::TextButton (
            buttonText
        )
    {
    }

    void paintButton (
        juce::Graphics& g,
        bool isMouseOverButton,
        bool isButtonDown) override
    {
        juce::TextButton::paintButton (
            g,
            isMouseOverButton,
            isButtonDown
        );

        if (! getToggleState())
            return;

        auto ring =
            getLocalBounds()
                .toFloat()
                .reduced (1.5f);

        g.setColour (
            juce::Colour::fromRGB (
                66,
                238,
                151
            )
            .withAlpha (0.26f)
        );

        g.drawRoundedRectangle (
            ring.reduced (2.0f),
            8.0f,
            5.0f
        );

        g.setColour (
            juce::Colour::fromRGB (
                66,
                238,
                151
            )
        );

        g.drawRoundedRectangle (
            ring,
            9.0f,
            2.2f
        );
    }
};

class AudioSettingsPanel :
    public juce::Component,
    private juce::Timer
{
public:
    AudioSettingsPanel (
        AudioSystem& audioSystemToUse,
        MidiSystem& midiSystemToUse,
        JADLookAndFeel& lookAndFeelToUse
    );

    ~AudioSettingsPanel() override;

    void paint (
        juce::Graphics& g
    ) override;

    void resized() override;

    std::function<void()> onClose;

private:
    void timerCallback() override;
    void refreshStatus();

    void setPreferencesPage (
        int page
    );

    void rebuildMidiInputList();
    void refreshMidiPage();

    AudioSystem& audioSystem;
    MidiSystem& midiSystem;
    JADLookAndFeel& jadLookAndFeel;

    juce::Label titleLabel;
    juce::Label statusLabel;
    juce::Label detailLabel;
    juce::Label placeholderLabel;

    juce::Label midiInputTitleLabel;
    juce::Label midiConnectionLabel;
    juce::Label midiActivityLabel;
    juce::Label midiHelpLabel;

    juce::ComboBox midiInputBox;

    juce::ToggleButton autoReconnectButton {
        "AUTO-CONNECT LAST USB MIDI DEVICE"
    };

    juce::TextButton refreshMidiButton {
        "REFRESH DEVICES"
    };

    PreferencesTabButton audioTabButton { "AUDIO" };
    PreferencesTabButton midiTabButton { "MIDI" };
    PreferencesTabButton recordingTabButton { "RECORDING" };
    PreferencesTabButton appearanceTabButton { "APPEARANCE" };
    PreferencesTabButton updatesTabButton { "UPDATES" };
    juce::TextButton closeButton { "X" };

    juce::AudioDeviceSelectorComponent deviceSelector;

    int activePreferencesPage = 0;
    int midiRefreshCounter = 0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (
        AudioSettingsPanel
    )
};
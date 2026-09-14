#pragma once

#include <juce_audio_utils/juce_audio_utils.h>
#include <juce_gui_basics/juce_gui_basics.h>

#include "../Audio/AudioSystem.h"
#include "JADLookAndFeel.h"

class AudioSettingsPanel :
    public juce::Component,
    private juce::Timer
{
public:
    AudioSettingsPanel (
        AudioSystem& audioSystemToUse,
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

    AudioSystem& audioSystem;
    JADLookAndFeel& jadLookAndFeel;

    juce::Label titleLabel;
    juce::Label statusLabel;
    juce::Label detailLabel;

    juce::TextButton closeButton {
        "X"
    };

    juce::AudioDeviceSelectorComponent
        deviceSelector;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (
        AudioSettingsPanel
    )
};

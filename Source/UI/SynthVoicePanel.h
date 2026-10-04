#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include "../Audio/AudioSystem.h"

class SynthVoicePanel : public juce::Component
{
public:
    explicit SynthVoicePanel (AudioSystem& audioSystem);
    ~SynthVoicePanel() override;

    void paint (juce::Graphics& g) override;
    void resized() override;

private:
    AudioSystem& audio;

    juce::TextButton voiceButton { "VOICE" };

    juce::Slider frequencySlider {
        juce::Slider::LinearHorizontal,
        juce::Slider::TextBoxRight
    };

    juce::Slider gainSlider {
        juce::Slider::LinearHorizontal,
        juce::Slider::TextBoxRight
    };

    juce::ComboBox waveformBox;

    juce::Label titleLabel;
    juce::Label waveformLabel;
    juce::Label frequencyLabel;
    juce::Label gainLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SynthVoicePanel)
};

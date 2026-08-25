#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "../Core/JADContext.h"

class MainComponent : public juce::Component
{
public:
    explicit MainComponent (JADContext& contextToUse);
    ~MainComponent() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    JADContext& context;

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

    juce::Label frequencyLabel;
    juce::Label gainLabel;
    juce::Label waveformLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};

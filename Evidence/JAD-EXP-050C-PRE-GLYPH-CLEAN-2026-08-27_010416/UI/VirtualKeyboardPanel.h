#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_utils/juce_audio_utils.h>

#include "../Audio/AudioSystem.h"
#include "../Audio/MidiSystem.h"

class JADLookAndFeel;

class VirtualKeyboardPanel :
    public juce::Component,
    private juce::Timer
{
public:
    VirtualKeyboardPanel (
        AudioSystem& audioSystemToUse,
        JADLookAndFeel& lookAndFeelToUse
    );

    void paint (
        juce::Graphics&
    ) override;

    void resized() override;

private:
    void timerCallback() override;

    void rebuildMidiDeviceList();
    void applyKeyboardTheme();

    AudioSystem& audioSystem;
    JADLookAndFeel& jadLookAndFeel;

    MidiSystem midiSystem;

    juce::MidiKeyboardComponent keyboard;

    juce::Label titleLabel;
    juce::Label activityLabel;
    juce::Label octaveLabel;
    juce::Label velocityLabel;

    juce::ComboBox midiDeviceBox;
    juce::ComboBox keyboardThemeBox;

    juce::TextButton octaveDownButton {
        "OCT -"
    };

    juce::TextButton octaveUpButton {
        "OCT +"
    };

    juce::TextButton refreshMidiButton {
        "REFRESH MIDI"
    };

    juce::Slider velocitySlider;

    int baseOctave = 3;
};

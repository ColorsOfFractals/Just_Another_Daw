#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_utils/juce_audio_utils.h>

#include "../Audio/AudioSystem.h"
#include "../Audio/MidiSystem.h"

#include "JADLookAndFeel.h"

class VirtualKeyboardPanel :
    public juce::Component,
    private juce::Timer
{
public:
    VirtualKeyboardPanel (
        AudioSystem& audioSystemToUse,
        JADLookAndFeel& lookAndFeelToUse
    );

    ~VirtualKeyboardPanel() override;

    void paint (
        juce::Graphics&
    ) override;

    void resized() override;

    bool keyPressed (
        const juce::KeyPress&
    ) override;

    bool keyStateChanged (
        bool isKeyDown
    ) override;

private:
    void timerCallback() override;

    void rebuildMidiDeviceList();
    void applyKeyboardTheme();

    void changeComputerOctave (
        int delta
    );

    void refreshComputerNotes();

    int getMidiNoteForComputerKey (
        juce_wchar key
    ) const;

    void drawComputerKeyboardLegend (
        juce::Graphics&,
        juce::Rectangle<int>
    );

    void drawOctaveMarkers (
        juce::Graphics&,
        juce::Rectangle<int>
    );

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

    juce::Array<int> heldComputerNotes;

    juce::Rectangle<int> keyboardBounds;
    juce::Rectangle<int> legendBounds;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (
        VirtualKeyboardPanel
    )
};
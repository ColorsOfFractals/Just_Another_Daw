#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include "JADLookAndFeel.h"

class PluginLibraryPanel :
    public juce::Component
{
public:
    explicit PluginLibraryPanel (
        JADLookAndFeel& lookAndFeelToUse
    );

    ~PluginLibraryPanel() override = default;

    void paint (
        juce::Graphics&
    ) override;

    void resized() override;

private:
    JADLookAndFeel& jadLookAndFeel;

    juce::Label titleLabel;
    juce::Label countLabel;
    juce::Label cabinetLabel;
    juce::Label detailLabel;

    juce::TextEditor searchBox;

    juce::TextButton scanButton {
        "SCAN VST3"
    };

    juce::TextButton allButton {
        "ALL"
    };

    juce::TextButton instrumentsButton {
        "INSTRUMENTS"
    };

    juce::TextButton effectsButton {
        "EFFECTS"
    };

    juce::TextButton favouritesButton {
        "FAVORITES"
    };

    juce::TextButton loadButton {
        "LOAD INTO TRACK"
    };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (
        PluginLibraryPanel
    )
};

#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include "../Audio/AudioSystem.h"
#include "../Plugins/PluginCatalog.h"
#include "../Model/SessionState.h"

#include <functional>
#include "JADLookAndFeel.h"

class PluginLibraryPanel :
    public juce::Component,
    private juce::ListBoxModel
{
public:
    PluginLibraryPanel (
        PluginCatalog& catalogToUse,
        AudioSystem& audioSystemToUse,
        SessionState& sessionStateToUse,
        JADLookAndFeel& lookAndFeelToUse
    );

    ~PluginLibraryPanel() override = default;

    void paint (
        juce::Graphics&
    ) override;

    void resized() override;

    std::function<void (int)> onTrackAssigned;

private:
    enum class Filter
    {
        all = 0,
        instruments,
        effects
    };

    int getNumRows() override;

    void paintListBoxItem (
        int rowNumber,
        juce::Graphics&,
        int width,
        int height,
        bool rowIsSelected
    ) override;

    void selectedRowsChanged (
        int lastRowSelected
    ) override;

    void refreshPlugins();

    void openPluginEditor();

    void chooseTrackForSelectedPlugin();

    void loadSelectedPluginIntoTrack (
        int trackIndex
    );

    void setFilter (
        Filter newFilter
    );

    bool passesFilter (
        const juce::PluginDescription&
    ) const;

    juce::String getPluginTypeText (
        const juce::PluginDescription&
    ) const;

    PluginCatalog& catalog;
    AudioSystem& audioSystem;
    SessionState& sessionState;
    JADLookAndFeel& jadLookAndFeel;

    juce::Array<juce::PluginDescription> visiblePlugins;

    int selectedPluginIndex = -1;
    int editorTrackIndex = -1;
Filter filter =
        Filter::all;

    juce::Label titleLabel;
    juce::Label countLabel;
    juce::Label cabinetLabel;
    juce::Label detailLabel;
    juce::Label scanStatusLabel;

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
        "ASSIGN PLUGIN"
    };

    juce::TextButton openEditorButton {
        "OPEN PLUGIN"
    };

    juce::ListBox pluginList {
        "Plugin Cabinet",
        this
    };

    std::unique_ptr<juce::DocumentWindow>
        pluginEditorWindow;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (
        PluginLibraryPanel
    )
};
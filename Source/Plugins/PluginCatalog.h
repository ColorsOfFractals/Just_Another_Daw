#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

class PluginCatalog
{
public:
    PluginCatalog();

    void initialise();
    void scanVST3();

    int getNumPlugins() const noexcept;

    juce::Array<juce::PluginDescription>
    getPlugins() const;

    juce::Array<juce::PluginDescription>
    search (
        const juce::String& query
    ) const;

    juce::File getCatalogFile() const;

    juce::String getLastScanSummary() const;

private:
    void load();
    void save();

    juce::AudioPluginFormatManager formatManager;
    juce::KnownPluginList knownPlugins;

    juce::String lastScanSummary;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (
        PluginCatalog
    )
};

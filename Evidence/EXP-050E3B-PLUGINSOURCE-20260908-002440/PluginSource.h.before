#pragma once

#include "AudioSource.h"

#include <juce_audio_processors/juce_audio_processors.h>

#include <memory>


class PluginSource final :
    public AudioSource
{
public:
    PluginSource() = default;

    explicit PluginSource (
        std::unique_ptr<juce::AudioPluginInstance> instanceToOwn
    );

    ~PluginSource() override;

    void setPlugin (
        std::unique_ptr<juce::AudioPluginInstance> instanceToOwn
    );

    void clearPlugin();

    bool hasPlugin() const noexcept;

    juce::AudioPluginInstance*
    getPlugin() noexcept;

    const juce::AudioPluginInstance*
    getPlugin() const noexcept;

    juce::String
    getPluginName() const;

    void prepare (
        double sampleRate,
        int maximumBlockSize
    ) override;

    void reset() override;

    void render (
        juce::AudioBuffer<float>& buffer,
        int numSamples
    ) override;

private:
    void preparePluginIfPossible();

    std::unique_ptr<juce::AudioPluginInstance> plugin;

    double currentSampleRate = 0.0;
    int currentMaximumBlockSize = 0;

    juce::MidiBuffer midiBuffer;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (
        PluginSource
    )
};

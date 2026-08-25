#pragma once

#include "AudioSource.h"

#include <juce_audio_basics/juce_audio_basics.h>

class TrackModel;

class TrackProcessor
{
public:
    TrackProcessor() = default;

    void setModel (
        TrackModel* modelToUse
    ) noexcept;

    void setSource (
        AudioSource* sourceToUse
    ) noexcept;

    void prepare (
        double sampleRate,
        int maximumBlockSize
    );

    void reset();

    void process (
        juce::AudioBuffer<float>& mixBuffer,
        int numSamples,
        bool anyTrackSoloed
    );

private:
    TrackModel* model = nullptr;
    AudioSource* source = nullptr;

    juce::AudioBuffer<float> workingBuffer;
};

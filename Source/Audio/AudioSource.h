#pragma once

#include <juce_audio_basics/juce_audio_basics.h>

class AudioSource
{
public:
    virtual ~AudioSource() = default;

    virtual void prepare (
        double sampleRate,
        int maximumBlockSize
    ) = 0;

    virtual void reset() = 0;

    virtual void render (
        juce::AudioBuffer<float>& destination,
        int numSamples
    ) = 0;
};

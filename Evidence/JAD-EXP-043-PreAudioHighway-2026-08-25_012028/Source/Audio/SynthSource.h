#pragma once

#include "AudioSource.h"

#include <juce_audio_basics/juce_audio_basics.h>

#include <atomic>

class SynthSource : public AudioSource
{
public:
    enum class Waveform
    {
        sine = 0,
        saw,
        square
    };

    void prepare (
        double sampleRate,
        int maximumBlockSize
    ) override;

    void reset() override;

    void render (
        juce::AudioBuffer<float>& destination,
        int numSamples
    ) override;

    void setGate (
        bool enabled
    ) noexcept;

    bool isGateEnabled() const noexcept;

    void setFrequency (
        double frequencyHz
    ) noexcept;

    double getFrequency() const noexcept;

    void setGain (
        float newGain
    ) noexcept;

    float getGain() const noexcept;

    void setWaveform (
        Waveform newWaveform
    ) noexcept;

    Waveform getWaveform() const noexcept;

private:
    void updateEnvelopeGate() noexcept;
    float renderOscillatorSample() noexcept;

    juce::ADSR envelope;

    std::atomic<bool> requestedGate { false };

    std::atomic<double> frequencyHz { 440.0 };
    std::atomic<float> outputGain { 0.05f };

    std::atomic<int> waveform {
        static_cast<int> (Waveform::sine)
    };

    double currentSampleRate = 0.0;
    double phase = 0.0;

    bool appliedGate = false;
};

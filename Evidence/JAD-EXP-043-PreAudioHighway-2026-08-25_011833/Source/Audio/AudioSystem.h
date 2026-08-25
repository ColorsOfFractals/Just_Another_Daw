#pragma once

#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_audio_devices/juce_audio_devices.h>

#include <atomic>

class TransportState;
class MasterBusState;

class AudioSystem : private juce::AudioIODeviceCallback
{
public:
    enum class Waveform
    {
        sine = 0,
        saw,
        square
    };

    AudioSystem();
    ~AudioSystem() override;

    bool isAlive() const noexcept;
    bool isReady() const noexcept;

    juce::String getOutputDeviceName() const;
    juce::String getInputDeviceName() const;

    double getSampleRate() const noexcept;
    int getBufferSize() const noexcept;

    void setTestToneEnabled (bool enabled) noexcept;
    bool isTestToneEnabled() const noexcept;

    void setTransportState (
        TransportState* transportStateToUse
    ) noexcept;

    void setMasterBusState (
        MasterBusState* masterBusStateToUse
    ) noexcept;

    void setFrequency (double frequencyHz) noexcept;
    double getFrequency() const noexcept;

    void setGain (float newGain) noexcept;
    float getGain() const noexcept;

    void setWaveform (Waveform newWaveform) noexcept;
    Waveform getWaveform() const noexcept;

private:
    void audioDeviceIOCallbackWithContext (
        const float* const* inputChannelData,
        int numInputChannels,
        float* const* outputChannelData,
        int numOutputChannels,
        int numSamples,
        const juce::AudioIODeviceCallbackContext& context
    ) override;

    void audioDeviceAboutToStart (
        juce::AudioIODevice* device
    ) override;

    void audioDeviceStopped() override;

    float renderOscillatorSample() noexcept;
    void updateEnvelopeGate() noexcept;

    TransportState* transportState = nullptr;
    MasterBusState* masterBusState = nullptr;

    juce::AudioDeviceManager deviceManager;
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

    bool alive = false;
    bool ready = false;
};




#pragma once

#include "SynthSource.h"
#include "TrackProcessor.h"

#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_audio_devices/juce_audio_devices.h>

#include <array>

class SessionState;
class TransportState;
class MasterBusState;

class AudioSystem : private juce::AudioIODeviceCallback
{
public:
    using Waveform = SynthSource::Waveform;

    AudioSystem();
    ~AudioSystem() override;

    bool isAlive() const noexcept;
    bool isReady() const noexcept;

    juce::String getOutputDeviceName() const;
    juce::String getInputDeviceName() const;

    double getSampleRate() const noexcept;
    int getBufferSize() const noexcept;

    juce::AudioDeviceManager&
    getDeviceManager() noexcept;

    void setTestToneEnabled (
        bool enabled
    ) noexcept;

    bool isTestToneEnabled() const noexcept;

    void setSessionState (
        SessionState* sessionStateToUse
    ) noexcept;

    void setTransportState (
        TransportState* transportStateToUse
    ) noexcept;

    void setMasterBusState (
        MasterBusState* masterBusStateToUse
    ) noexcept;

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

    void rebuildTrackRoutes();

    SessionState* sessionState = nullptr;
    TransportState* transportState = nullptr;
    MasterBusState* masterBusState = nullptr;

    juce::AudioDeviceManager deviceManager;

    SynthSource synthSource;

    std::array<
        TrackProcessor,
        4
    > trackProcessors;

    juce::AudioBuffer<float> masterMixBuffer;

    double currentSampleRate = 0.0;
    int currentBlockSize = 0;

    bool alive = false;
    bool ready = false;
};


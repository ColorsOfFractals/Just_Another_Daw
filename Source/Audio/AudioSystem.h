#pragma once

#include "PluginSource.h"
#include "SynthSource.h"
#include "TrackProcessor.h"

#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_audio_devices/juce_audio_devices.h>

#include <array>
#include <atomic>

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

    double getTransportTempo() const noexcept;

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

    void handleMidiMessage (
        const juce::MidiMessage& message
    );

    static constexpr int maxTrackCount = 16;

    void setMidiTargetTrack (
        int trackIndex
    ) noexcept;

    int getMidiTargetTrack() const noexcept;

    bool installTrackPlugin (
        int trackIndex,
        std::unique_ptr<juce::AudioPluginInstance> instance
    );

    void restoreNativeSynth (
        int trackIndex = 0
    );

    bool hasTrackPlugin (
        int trackIndex
    ) const noexcept;

    juce::String getTrackPluginName (
        int trackIndex
    ) const;

    juce::AudioPluginInstance*
    getTrackPlugin (
        int trackIndex
    ) noexcept;

    void refreshTrackRoutes();

    void handleTrackRemoved (
        int removedTrackIndex
    );

private:
    void renderMetronome (
        juce::AudioBuffer<float>& destination,
        int numSamples,
        double blockStartBeat
    ) noexcept;

    void triggerMetronomeClick (
        bool accented
    ) noexcept;

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
        PluginSource,
        maxTrackCount
    > pluginSources;

    std::array<
        TrackProcessor,
        maxTrackCount
    > trackProcessors;

    std::atomic<int> midiTargetTrackIndex { 0 };
    std::atomic<bool> recordingCaptureActive { false };

    std::int64_t lastMetronomeBeat { -1 };

    int metronomeSamplesRemaining { 0 };
    int metronomeClickLengthSamples { 1 };

    double metronomePhase { 0.0 };
    double metronomeFrequency { 1400.0 };

    float metronomeClickAmplitude { 0.0f };

    std::uint32_t metronomeNoiseState {
        0x51f15e5u
    };

    juce::AudioBuffer<float> masterMixBuffer;

    double currentSampleRate = 0.0;
    int currentBlockSize = 0;

    bool alive = false;
    bool ready = false;
};


#pragma once

#include <atomic>
#include <cstdint>

class TransportState
{
public:
    TransportState() = default;

    // ------------------------------------------------------------
    // TRANSPORT COMMANDS
    // ------------------------------------------------------------

    void play() noexcept;
    void stop() noexcept;

    void beginRecording() noexcept;
    void endRecording() noexcept;

    void returnToStart() noexcept;

    // ------------------------------------------------------------
    // CLOCK
    // ------------------------------------------------------------

    void setTempo (double beatsPerMinute) noexcept;
    double getTempo() const noexcept;

    void setPositionInBeats (double beats) noexcept;
    double getPositionInBeats() const noexcept;

    void setPositionInSamples (std::int64_t samples) noexcept;
    std::int64_t getPositionInSamples() const noexcept;

    void advanceSamples (
        int numSamples,
        double sampleRate
    ) noexcept;

    // ------------------------------------------------------------
    // STATE
    // ------------------------------------------------------------

    bool isPlaying() const noexcept;
    bool isRecording() const noexcept;

    // ------------------------------------------------------------
    // MUSICAL GRID
    // ------------------------------------------------------------

    void setTimeSignature (
        int numerator,
        int denominator
    ) noexcept;

    int getTimeSignatureNumerator() const noexcept;
    int getTimeSignatureDenominator() const noexcept;

    // ------------------------------------------------------------
    // METRONOME SETTINGS
    // ------------------------------------------------------------

    void setMetronomeEnabled (bool enabled) noexcept;
    bool isMetronomeEnabled() const noexcept;

    void setCountInBars (int bars) noexcept;
    int getCountInBars() const noexcept;

    void setClickDuringPlayback (bool enabled) noexcept;
    bool isClickDuringPlaybackEnabled() const noexcept;

    void setClickDuringRecording (bool enabled) noexcept;
    bool isClickDuringRecordingEnabled() const noexcept;

    void setAccentFirstBeat (bool enabled) noexcept;
    bool isAccentFirstBeatEnabled() const noexcept;

    void setMetronomeVolume (float volume) noexcept;
    float getMetronomeVolume() const noexcept;

    void setMetronomeSound (int soundIndex) noexcept;
    int getMetronomeSound() const noexcept;

    // ------------------------------------------------------------
    // LOOP TERRITORY
    // ------------------------------------------------------------

    void setLoopEnabled (bool enabled) noexcept;
    bool isLoopEnabled() const noexcept;

    void setLoopRange (
        double startBeat,
        double endBeat
    ) noexcept;

    double getLoopStartBeat() const noexcept;
    double getLoopEndBeat() const noexcept;

private:
    std::atomic<bool> playing   { false };
    std::atomic<bool> recording { false };

    std::atomic<double> tempoBpm { 120.0 };

    std::atomic<double> positionBeats { 0.0 };
    std::atomic<std::int64_t> positionSamples { 0 };

    std::atomic<int> timeSignatureNumerator   { 4 };
    std::atomic<int> timeSignatureDenominator { 4 };

    std::atomic<bool> metronomeEnabled { false };
    std::atomic<int> countInBars { 2 };

    std::atomic<bool> clickDuringPlayback { true };
    std::atomic<bool> clickDuringRecording { true };
    std::atomic<bool> accentFirstBeat { true };

    std::atomic<float> metronomeVolume { 0.62f };
    std::atomic<int> metronomeSound { 0 };

    std::atomic<bool> loopEnabled { false };
    std::atomic<double> loopStartBeat { 0.0 };
    std::atomic<double> loopEndBeat   { 4.0 };
};
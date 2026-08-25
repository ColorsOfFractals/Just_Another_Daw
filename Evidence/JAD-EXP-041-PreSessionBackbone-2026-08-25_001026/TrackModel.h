#pragma once

#include <atomic>
#include <cstdint>
#include <string>

class TrackModel
{
public:
    using TrackId = std::uint64_t;

    explicit TrackModel (
        TrackId trackId = 1,
        std::string trackName = "Track 1"
    );

    // ------------------------------------------------------------
    // IDENTITY
    // ------------------------------------------------------------

    TrackId getId() const noexcept;

    void setName (
        std::string newName
    );

    const std::string& getName() const noexcept;

    // ------------------------------------------------------------
    // MIX STATE
    // ------------------------------------------------------------

    void setGain (
        float linearGain
    ) noexcept;

    float getGain() const noexcept;

    void setPan (
        float panPosition
    ) noexcept;

    float getPan() const noexcept;

    // ------------------------------------------------------------
    // TRACK SWITCHES
    // ------------------------------------------------------------

    void setMuted (
        bool shouldBeMuted
    ) noexcept;

    bool isMuted() const noexcept;

    void setSolo (
        bool shouldBeSolo
    ) noexcept;

    bool isSolo() const noexcept;

    void setRecordArmed (
        bool shouldBeArmed
    ) noexcept;

    bool isRecordArmed() const noexcept;

private:
    TrackId id;
    std::string name;

    std::atomic<float> gain { 1.0f };
    std::atomic<float> pan  { 0.0f };

    std::atomic<bool> muted      { false };
    std::atomic<bool> solo       { false };
    std::atomic<bool> recordArmed { false };
};

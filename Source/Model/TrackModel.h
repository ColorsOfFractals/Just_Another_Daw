#pragma once

#include "ClipModel.h"

#include <atomic>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

class TrackModel
{
public:
    using TrackId = std::uint64_t;

    struct RecordedMidiEvent
    {
        bool noteOn = false;
        int noteNumber = 0;
        float velocity = 0.0f;
        int channel = 1;
        double beat = 0.0;
    };

    explicit TrackModel (
        TrackId trackId = 1,
        std::string trackName = "Track 1"
    );

    TrackId getId() const noexcept;

    void setName (std::string newName);
    const std::string& getName() const noexcept;

    void setGain (float linearGain) noexcept;
    float getGain() const noexcept;

    void setPan (float panPosition) noexcept;
    float getPan() const noexcept;

    void setMuted (bool shouldBeMuted) noexcept;
    bool isMuted() const noexcept;

    void setSolo (bool shouldBeSolo) noexcept;
    bool isSolo() const noexcept;

    void setRecordArmed (bool shouldBeArmed) noexcept;
    bool isRecordArmed() const noexcept;

    void clearRecordedMidiEvents();

    void recordMidiEvent (
        bool noteOn,
        int noteNumber,
        float velocity,
        int channel,
        double beat
    );

    std::size_t getRecordedMidiEventCount() const;

    std::vector<RecordedMidiEvent>
    getRecordedMidiEventsSnapshot() const;

    // ------------------------------------------------------------
    // CLIPS
    // ------------------------------------------------------------

    ClipModel& addClip (
        std::string name,
        double startBeat,
        double lengthBeats
    );

    bool removeClip (
        ClipModel::ClipId clipId
    );

    std::size_t getClipCount() const noexcept;

    ClipModel* getClip (
        std::size_t index
    ) noexcept;

    const ClipModel* getClip (
        std::size_t index
    ) const noexcept;

private:
    TrackId id;
    std::string name;

    std::atomic<float> gain { 1.0f };
    std::atomic<float> pan  { 0.0f };

    std::atomic<bool> muted { false };
    std::atomic<bool> solo { false };
    std::atomic<bool> recordArmed { false };

    mutable std::mutex recordedMidiMutex;
    std::vector<RecordedMidiEvent> recordedMidiEvents;

    std::vector<
        std::unique_ptr<ClipModel>
    > clips;

    ClipModel::ClipId nextClipId { 1 };
};

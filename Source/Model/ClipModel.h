#pragma once

#include <atomic>
#include <cstddef>
#include <cstdint>
#include <mutex>
#include <string>
#include <vector>

class ClipModel
{
public:
    using ClipId = std::uint64_t;

    struct MidiEvent
    {
        bool noteOn = false;
        int noteNumber = 0;
        float velocity = 0.0f;
        int channel = 1;
        double beat = 0.0;
    };

    ClipModel (
        ClipId clipId,
        std::string clipName,
        double startBeat,
        double lengthBeats
    );

    ClipId getId() const noexcept;

    const std::string& getName() const noexcept;
    void setName (std::string newName);

    double getStartBeat() const noexcept;
    void setStartBeat (double beat) noexcept;

    double getLengthBeats() const noexcept;
    void setLengthBeats (double beats) noexcept;

    bool isMuted() const noexcept;
    void setMuted (bool shouldBeMuted) noexcept;

    void setMidiEvents (
        std::vector<MidiEvent> events
    );

    std::vector<MidiEvent>
    getMidiEventsSnapshot() const;

    std::size_t getMidiEventCount() const;

    std::size_t quantizeMidi (
        double gridBeats
    );

private:
    ClipId id;
    std::string name;

    std::atomic<double> startBeat { 0.0 };
    std::atomic<double> lengthBeats { 1.0 };
    std::atomic<bool> muted { false };

    mutable std::mutex midiMutex;
    std::vector<MidiEvent> midiEvents;
};

#pragma once

#include <atomic>
#include <cstdint>
#include <string>

class ClipModel
{
public:
    using ClipId = std::uint64_t;

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

private:
    ClipId id;
    std::string name;

    std::atomic<double> startBeat { 0.0 };
    std::atomic<double> lengthBeats { 1.0 };
    std::atomic<bool> muted { false };
};

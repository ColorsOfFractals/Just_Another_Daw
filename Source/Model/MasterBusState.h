#pragma once

#include <atomic>

class MasterBusState
{
public:
    void setGain (float linearGain) noexcept;
    float getGain() const noexcept;

    void setMuted (bool shouldBeMuted) noexcept;
    bool isMuted() const noexcept;

private:
    std::atomic<float> gain { 1.0f };
    std::atomic<bool> muted { false };
};

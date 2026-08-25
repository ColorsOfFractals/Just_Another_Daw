#include "MasterBusState.h"

#include <algorithm>

void MasterBusState::setGain (
    float linearGain) noexcept
{
    gain.store (
        std::clamp (
            linearGain,
            0.0f,
            2.0f
        )
    );
}

float MasterBusState::getGain() const noexcept
{
    return gain.load();
}

void MasterBusState::setMuted (
    bool shouldBeMuted) noexcept
{
    muted.store (shouldBeMuted);
}

bool MasterBusState::isMuted() const noexcept
{
    return muted.load();
}

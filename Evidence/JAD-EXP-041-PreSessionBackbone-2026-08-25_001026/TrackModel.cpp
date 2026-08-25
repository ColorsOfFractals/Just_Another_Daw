#include "TrackModel.h"

#include <algorithm>
#include <utility>

TrackModel::TrackModel (
    TrackId trackId,
    std::string trackName)
    : id (trackId),
      name (std::move (trackName))
{
}

TrackModel::TrackId TrackModel::getId() const noexcept
{
    return id;
}

void TrackModel::setName (
    std::string newName)
{
    name =
        std::move (
            newName
        );
}

const std::string& TrackModel::getName() const noexcept
{
    return name;
}

void TrackModel::setGain (
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

float TrackModel::getGain() const noexcept
{
    return gain.load();
}

void TrackModel::setPan (
    float panPosition) noexcept
{
    pan.store (
        std::clamp (
            panPosition,
            -1.0f,
            1.0f
        )
    );
}

float TrackModel::getPan() const noexcept
{
    return pan.load();
}

void TrackModel::setMuted (
    bool shouldBeMuted) noexcept
{
    muted.store (
        shouldBeMuted
    );
}

bool TrackModel::isMuted() const noexcept
{
    return muted.load();
}

void TrackModel::setSolo (
    bool shouldBeSolo) noexcept
{
    solo.store (
        shouldBeSolo
    );
}

bool TrackModel::isSolo() const noexcept
{
    return solo.load();
}

void TrackModel::setRecordArmed (
    bool shouldBeArmed) noexcept
{
    recordArmed.store (
        shouldBeArmed
    );
}

bool TrackModel::isRecordArmed() const noexcept
{
    return recordArmed.load();
}

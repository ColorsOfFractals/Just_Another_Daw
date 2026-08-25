#include "ClipModel.h"

#include <algorithm>
#include <utility>

ClipModel::ClipModel (
    ClipId clipId,
    std::string clipName,
    double initialStartBeat,
    double initialLengthBeats)
    : id (clipId),
      name (std::move (clipName))
{
    setStartBeat (initialStartBeat);
    setLengthBeats (initialLengthBeats);
}

ClipModel::ClipId ClipModel::getId() const noexcept
{
    return id;
}

const std::string& ClipModel::getName() const noexcept
{
    return name;
}

void ClipModel::setName (std::string newName)
{
    name = std::move (newName);
}

double ClipModel::getStartBeat() const noexcept
{
    return startBeat.load();
}

void ClipModel::setStartBeat (double beat) noexcept
{
    startBeat.store (
        std::max (0.0, beat)
    );
}

double ClipModel::getLengthBeats() const noexcept
{
    return lengthBeats.load();
}

void ClipModel::setLengthBeats (double beats) noexcept
{
    lengthBeats.store (
        std::max (0.001, beats)
    );
}

bool ClipModel::isMuted() const noexcept
{
    return muted.load();
}

void ClipModel::setMuted (bool shouldBeMuted) noexcept
{
    muted.store (shouldBeMuted);
}

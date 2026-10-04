#include "SessionState.h"

#include <algorithm>
#include <utility>

SessionState::SessionState()
{
    projectState.setTitle (
        "First JAD Session"
    );

    projectState.markSaved();

    auto& track1 = addTrack ("Track 1");
    auto& track2 = addTrack ("Track 2");
    auto& track3 = addTrack ("Track 3");
    auto& track4 = addTrack ("Track 4");

    track1.addClip (
        "Seed A",
        0.0,
        4.0
    );

    track2.addClip (
        "Seed B",
        4.0,
        4.0
    );

    track3.addClip (
        "Seed C",
        8.0,
        4.0
    );

    track4.addClip (
        "Seed D",
        12.0,
        4.0
    );
}

TrackModel& SessionState::addTrack (
    std::string name)
{
    const auto id =
        nextTrackId++;

    if (name.empty())
    {
        name =
            "Track "
            + std::to_string (id);
    }

    auto track =
        std::make_unique<TrackModel> (
            id,
            std::move (name)
        );

    auto& reference =
        *track;

    tracks.push_back (
        std::move (track)
    );

    projectState.markDirty();

    return reference;
}

bool SessionState::removeTrack (
    TrackModel::TrackId trackId)
{
    const auto previousSize =
        tracks.size();

    tracks.erase (
        std::remove_if (
            tracks.begin(),
            tracks.end(),
            [trackId] (
                const std::unique_ptr<TrackModel>& track)
            {
                return track != nullptr
                    && track->getId() == trackId;
            }
        ),
        tracks.end()
    );

    const auto removed =
        tracks.size() != previousSize;

    if (removed)
        projectState.markDirty();

    return removed;
}

void SessionState::clearTracks()
{
    tracks.clear();
    projectState.markDirty();
}

std::size_t SessionState::getTrackCount() const noexcept
{
    return tracks.size();
}

TrackModel* SessionState::getTrack (
    std::size_t index) noexcept
{
    if (index >= tracks.size())
        return nullptr;

    return tracks[index].get();
}

const TrackModel* SessionState::getTrack (
    std::size_t index) const noexcept
{
    if (index >= tracks.size())
        return nullptr;

    return tracks[index].get();
}

TrackModel* SessionState::findTrackById (
    TrackModel::TrackId trackId) noexcept
{
    for (auto& track : tracks)
    {
        if (
            track != nullptr
            && track->getId() == trackId
        )
        {
            return track.get();
        }
    }

    return nullptr;
}

const TrackModel* SessionState::findTrackById (
    TrackModel::TrackId trackId) const noexcept
{
    for (const auto& track : tracks)
    {
        if (
            track != nullptr
            && track->getId() == trackId
        )
        {
            return track.get();
        }
    }

    return nullptr;
}

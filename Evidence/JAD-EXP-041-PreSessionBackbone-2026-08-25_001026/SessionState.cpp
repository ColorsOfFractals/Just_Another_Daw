#include "SessionState.h"

#include <algorithm>
#include <utility>

SessionState::SessionState()
{
    addTrack ("Track 1");
    addTrack ("Track 2");
    addTrack ("Track 3");
    addTrack ("Track 4");
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

    return tracks.size()
        != previousSize;
}

void SessionState::clearTracks()
{
    tracks.clear();
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

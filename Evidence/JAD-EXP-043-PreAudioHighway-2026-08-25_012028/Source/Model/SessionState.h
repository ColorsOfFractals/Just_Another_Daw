#pragma once

#include "MasterBusState.h"
#include "ProjectState.h"
#include "TrackModel.h"
#include "TransportState.h"

#include <cstddef>
#include <memory>
#include <string>
#include <vector>

class SessionState
{
public:
    SessionState();

    bool isAlive() const noexcept
    {
        return true;
    }

    TransportState& getTransportState() noexcept
    {
        return transportState;
    }

    const TransportState& getTransportState() const noexcept
    {
        return transportState;
    }

    MasterBusState& getMasterBusState() noexcept
    {
        return masterBusState;
    }

    const MasterBusState& getMasterBusState() const noexcept
    {
        return masterBusState;
    }

    ProjectState& getProjectState() noexcept
    {
        return projectState;
    }

    const ProjectState& getProjectState() const noexcept
    {
        return projectState;
    }

    TrackModel& addTrack (
        std::string name = {}
    );

    bool removeTrack (
        TrackModel::TrackId trackId
    );

    void clearTracks();

    std::size_t getTrackCount() const noexcept;

    TrackModel* getTrack (
        std::size_t index
    ) noexcept;

    const TrackModel* getTrack (
        std::size_t index
    ) const noexcept;

    TrackModel* findTrackById (
        TrackModel::TrackId trackId
    ) noexcept;

    const TrackModel* findTrackById (
        TrackModel::TrackId trackId
    ) const noexcept;

private:
    TransportState transportState;
    MasterBusState masterBusState;
    ProjectState projectState;

    std::vector<
        std::unique_ptr<TrackModel>
    > tracks;

    TrackModel::TrackId nextTrackId { 1 };
};

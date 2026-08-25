#pragma once

#include "TransportState.h"
#include "TrackModel.h"
class SessionState
{
public:

    TransportState& getTransportState() noexcept
    {
        return transportState;
    }

    const TransportState& getTransportState() const noexcept
    {
        return transportState;
    }
    SessionState();

    bool isAlive() const noexcept;


    TrackModel& getTrackModel() noexcept
    {
        return trackModel;
    }

    const TrackModel& getTrackModel() const noexcept
    {
        return trackModel;
    }
private:
    TransportState transportState;
    TrackModel trackModel { 1, "Track 1" };

    bool alive = false;
};



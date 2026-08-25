#pragma once

#include "TransportState.h"
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

private:
    TransportState transportState;

    bool alive = false;
};


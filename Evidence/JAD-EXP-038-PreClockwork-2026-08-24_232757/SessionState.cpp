#include "SessionState.h"

SessionState::SessionState()
{
    alive = true;
}

bool SessionState::isAlive() const noexcept
{
    return alive;
}

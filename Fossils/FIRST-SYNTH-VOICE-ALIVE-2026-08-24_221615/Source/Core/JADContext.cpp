#include "JADContext.h"

JADContext::JADContext() = default;

AudioSystem& JADContext::getAudioSystem() noexcept
{
    return audioSystem;
}

SessionState& JADContext::getSessionState() noexcept
{
    return sessionState;
}

bool JADContext::isAlive() const noexcept
{
    return audioSystem.isAlive()
        && sessionState.isAlive();
}

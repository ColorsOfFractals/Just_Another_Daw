#include "JADContext.h"

JADContext::JADContext()
{
    audioSystem.setTransportState (
        &sessionState.getTransportState()
    );

    audioSystem.setMasterBusState (
        &sessionState.getMasterBusState()
    );
}

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


#include "JADContext.h"

JADContext::JADContext()
{
    audioSystem.setSessionState (
        &sessionState
    );

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



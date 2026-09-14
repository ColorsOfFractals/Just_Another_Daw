#include "JADContext.h"

JADContext::JADContext()
    : midiSystem (
        audioSystem
    )
{
    pluginCatalog.initialise();
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

PluginCatalog&
JADContext::getPluginCatalog() noexcept
{
    return pluginCatalog;
}
MidiSystem&
JADContext::getMidiSystem() noexcept
{
    return midiSystem;
}



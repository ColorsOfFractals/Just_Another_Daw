#pragma once

#include "../Audio/AudioSystem.h"
#include "../Model/SessionState.h"

class JADContext
{
public:
    JADContext();

    AudioSystem& getAudioSystem() noexcept;
    SessionState& getSessionState() noexcept;

    bool isAlive() const noexcept;

private:
    AudioSystem audioSystem;
    SessionState sessionState;
};

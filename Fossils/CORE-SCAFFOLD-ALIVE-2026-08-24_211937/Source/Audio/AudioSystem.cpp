#include "AudioSystem.h"

AudioSystem::AudioSystem()
{
    alive = true;
}

AudioSystem::~AudioSystem()
{
    alive = false;
}

bool AudioSystem::isAlive() const noexcept
{
    return alive;
}

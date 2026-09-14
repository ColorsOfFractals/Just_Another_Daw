#pragma once

#include "../Audio/AudioSystem.h"
#include "../Model/SessionState.h"
#include "../Plugins/PluginCatalog.h"

class JADContext
{
public:
    JADContext();

    AudioSystem& getAudioSystem() noexcept;
    SessionState& getSessionState() noexcept;
    PluginCatalog& getPluginCatalog() noexcept;

    bool isAlive() const noexcept;

private:
    AudioSystem audioSystem;
    SessionState sessionState;
    PluginCatalog pluginCatalog;
};

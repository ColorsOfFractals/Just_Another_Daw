#pragma once

#include "../Audio/AudioSystem.h"
#include "../Audio/MidiSystem.h"
#include "../Model/SessionState.h"
#include "../Plugins/PluginCatalog.h"

class JADContext
{
public:
    JADContext();

    AudioSystem& getAudioSystem() noexcept;

    MidiSystem& getMidiSystem() noexcept;
    SessionState& getSessionState() noexcept;
    PluginCatalog& getPluginCatalog() noexcept;

    bool isAlive() const noexcept;

private:
    AudioSystem audioSystem;
    MidiSystem midiSystem;
    SessionState sessionState;
    PluginCatalog pluginCatalog;
};



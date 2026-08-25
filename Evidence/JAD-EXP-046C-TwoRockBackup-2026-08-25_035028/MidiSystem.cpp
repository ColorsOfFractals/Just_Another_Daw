#include "MidiSystem.h"

#include "AudioSystem.h"

#include <cmath>

MidiSystem::MidiSystem (
    AudioSystem& audioSystemToUse)
    : audioSystem (
        audioSystemToUse
    )
{
    refreshDevices();
}

MidiSystem::~MidiSystem()
{
    closeDevice();
}

juce::MidiKeyboardState&
MidiSystem::getKeyboardState() noexcept
{
    return keyboardState;
}

void MidiSystem::refreshDevices()
{
    availableInputs =
        juce::MidiInput::getAvailableDevices();
}

int MidiSystem::getDeviceCount() const noexcept
{
    return static_cast<int> (
        availableInputs.size()
    );
}

juce::String MidiSystem::getDeviceName (
    int index) const
{
    if (
        index < 0
        || index >= getDeviceCount()
    )
    {
        return {};
    }

    return availableInputs[
        static_cast<std::size_t> (index)
    ].name;
}

bool MidiSystem::openDevice (
    int index)
{
    closeDevice();

    if (
        index < 0
        || index >= getDeviceCount()
    )
    {
        return false;
    }

    openInput =
        juce::MidiInput::openDevice (
            availableInputs[
                static_cast<std::size_t> (index)
            ].identifier,
            this
        );

    if (openInput == nullptr)
        return false;

    openInput->start();

    return true;
}

void MidiSystem::closeDevice()
{
    if (openInput != nullptr)
        openInput->stop();

    openInput.reset();
}

bool MidiSystem::isDeviceOpen() const noexcept
{
    return openInput != nullptr;
}

int MidiSystem::getLastNote() const noexcept
{
    return lastNote.load();
}

float MidiSystem::getLastVelocity() const noexcept
{
    return lastVelocity.load();
}

void MidiSystem::playVirtualNote (
    int midiNote,
    float velocity)
{
    const auto message =
        juce::MidiMessage::noteOn (
            1,
            midiNote,
            velocity
        );

    keyboardState.processNextMidiEvent (
        message
    );

    applyMessage (
        message
    );
}

void MidiSystem::releaseVirtualNote (
    int midiNote)
{
    const auto message =
        juce::MidiMessage::noteOff (
            1,
            midiNote
        );

    keyboardState.processNextMidiEvent (
        message
    );

    applyMessage (
        message
    );
}

void MidiSystem::handleIncomingMidiMessage (
    juce::MidiInput*,
    const juce::MidiMessage& message)
{
    keyboardState.processNextMidiEvent (
        message
    );

    applyMessage (
        message
    );
}

void MidiSystem::applyMessage (
    const juce::MidiMessage& message)
{
    if (message.isNoteOn())
    {
        const auto note =
            message.getNoteNumber();

        const auto velocity =
            message.getFloatVelocity();

        lastNote.store (
            note
        );

        lastVelocity.store (
            velocity
        );

        const auto frequency =
            440.0
            * std::pow (
                2.0,
                (
                    static_cast<double> (note)
                    - 69.0
                ) / 12.0
            );

        audioSystem.setFrequency (
            frequency
        );

        audioSystem.setTestToneEnabled (
            true
        );
    }

    if (message.isNoteOff())
    {
        lastNote.store (-1);
        lastVelocity.store (0.0f);

        audioSystem.setTestToneEnabled (
            false
        );
    }
}

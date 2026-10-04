#include "MidiSystem.h"

#include "AudioSystem.h"

#include <cmath>

MidiSystem::MidiSystem (
    AudioSystem& audioSystemToUse)
    : audioSystem (
        audioSystemToUse
    )
{
    keyboardState.addListener (
        this
    );

    loadMidiPreferences();
    refreshDevices();
}

MidiSystem::~MidiSystem()
{
    stopTimer();

    if (currentArpeggiatorNote >= 0)
    {
        sendArpeggiatorMessage (
            juce::MidiMessage::noteOff (
                1,
                currentArpeggiatorNote
            )
        );

        currentArpeggiatorNote = -1;
    }

    closeDevice();

    keyboardState.removeListener (
        this
    );
}


juce::File MidiSystem::getMidiPreferencesFile() const
{
    auto directory =
        juce::File::getSpecialLocation (
            juce::File::userApplicationDataDirectory
        )
        .getChildFile ("JAD");

    directory.createDirectory();

    return directory.getChildFile (
        "MidiPreferences.xml"
    );
}

void MidiSystem::loadMidiPreferences()
{
    const auto file =
        getMidiPreferencesFile();

    if (! file.existsAsFile())
        return;

    auto xml =
        juce::XmlDocument::parse (file);

    if (xml == nullptr)
        return;

    preferredDeviceIdentifier =
        xml->getStringAttribute (
            "preferredDevice"
        );

    autoReconnectEnabled.store (
        xml->getBoolAttribute (
            "autoReconnect",
            true
        )
    );
}

void MidiSystem::saveMidiPreferences() const
{
    juce::XmlElement xml (
        "JAD_MIDI_PREFERENCES"
    );

    xml.setAttribute (
        "preferredDevice",
        preferredDeviceIdentifier
    );

    xml.setAttribute (
        "autoReconnect",
        autoReconnectEnabled.load()
    );

    xml.writeTo (
        getMidiPreferencesFile()
    );
}
juce::MidiKeyboardState&
MidiSystem::getKeyboardState() noexcept
{
    return keyboardState;
}

void MidiSystem::refreshDevices()
{
    const auto devices =
        juce::MidiInput::getAvailableDevices();

    availableInputs.assign (
        devices.begin(),
        devices.end()
    );

    bool openDeviceStillExists =
        openDeviceIdentifier.isEmpty();

    for (const auto& device : availableInputs)
    {
        if (
            device.identifier
            == openDeviceIdentifier
        )
        {
            openDeviceStillExists = true;
            break;
        }
    }

    if (! openDeviceStillExists)
        closeDevice();

    if (
        openInput == nullptr
        && autoReconnectEnabled.load()
        && preferredDeviceIdentifier.isNotEmpty()
    )
    {
        for (
            int index = 0;
            index < getDeviceCount();
            ++index
        )
        {
            if (
                getDeviceIdentifier (index)
                == preferredDeviceIdentifier
            )
            {
                openDevice (index);
                break;
            }
        }
    }
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

juce::String MidiSystem::getDeviceIdentifier (
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
    ].identifier;
}

juce::String
MidiSystem::getOpenDeviceName() const
{
    return openDeviceName;
}

juce::String
MidiSystem::getOpenDeviceIdentifier() const
{
    return openDeviceIdentifier;
}

juce::String
MidiSystem::getPreferredDeviceIdentifier() const
{
    return preferredDeviceIdentifier;
}

bool MidiSystem::isPreferredDeviceAvailable() const
{
    if (preferredDeviceIdentifier.isEmpty())
        return true;

    for (const auto& device : availableInputs)
    {
        if (
            device.identifier
            == preferredDeviceIdentifier
        )
        {
            return true;
        }
    }

    return false;
}

void MidiSystem::selectComputerKeyboardOnly()
{
    preferredDeviceIdentifier.clear();
    closeDevice();
    saveMidiPreferences();
}

void MidiSystem::setAutoReconnectEnabled (
    bool shouldReconnect)
{
    autoReconnectEnabled.store (
        shouldReconnect
    );

    saveMidiPreferences();

    if (shouldReconnect)
        refreshDevices();
}

bool MidiSystem::isAutoReconnectEnabled() const noexcept
{
    return autoReconnectEnabled.load();
}

bool MidiSystem::openDevice (
    int index)
{
    if (
        index < 0
        || index >= getDeviceCount()
    )
    {
        return false;
    }

    const auto selectedDevice =
        availableInputs[
            static_cast<std::size_t> (index)
        ];

    closeDevice();

    preferredDeviceIdentifier =
        selectedDevice.identifier;

    openInput =
        juce::MidiInput::openDevice (
            selectedDevice.identifier,
            this
        );

    if (openInput == nullptr)
    {
        saveMidiPreferences();
        return false;
    }

    openDeviceIdentifier =
        selectedDevice.identifier;

    openDeviceName =
        selectedDevice.name;

    openInput->start();
    saveMidiPreferences();

    return true;
}

void MidiSystem::closeDevice()
{
    if (openInput != nullptr)
        openInput->stop();

    openInput.reset();

    openDeviceIdentifier.clear();
    openDeviceName.clear();
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

bool MidiSystem::isMidiActive() const noexcept
{
    return activeNoteCount.load() > 0;
}

int MidiSystem::getActiveNoteCount() const noexcept
{
    return activeNoteCount.load();
}

juce::uint32 MidiSystem::getActivitySerial() const noexcept
{
    return activitySerial.load();
}


void MidiSystem::setArpeggiatorEnabled (
    bool shouldBeEnabled)
{
    const auto wasEnabled =
        arpeggiatorEnabled.exchange (
            shouldBeEnabled
        );

    if (wasEnabled == shouldBeEnabled)
        return;

    juce::Array<int> heldNotes;
    juce::Array<float> heldVelocities;

    {
        const juce::ScopedLock lock (
            arpeggiatorLock
        );

        for (int note = 0; note < 128; ++note)
        {
            if (! heldArpeggiatorNotes[
                    static_cast<std::size_t> (note)
                ])
            {
                continue;
            }

            heldNotes.add (note);

            heldVelocities.add (
                heldArpeggiatorVelocities[
                    static_cast<std::size_t> (note)
                ]
            );
        }
    }

    if (currentArpeggiatorNote >= 0)
    {
        sendArpeggiatorMessage (
            juce::MidiMessage::noteOff (
                1,
                currentArpeggiatorNote
            )
        );

        currentArpeggiatorNote = -1;
    }

    arpeggiatorStep = 0;

    if (shouldBeEnabled)
    {
        for (const auto note : heldNotes)
        {
            sendArpeggiatorMessage (
                juce::MidiMessage::noteOff (
                    1,
                    note
                )
            );
        }

        startTimer (1);
        return;
    }

    stopTimer();

    for (int i = 0; i < heldNotes.size(); ++i)
    {
        sendArpeggiatorMessage (
            juce::MidiMessage::noteOn (
                1,
                heldNotes.getUnchecked (i),
                heldVelocities.getUnchecked (i)
            )
        );
    }
}


bool MidiSystem::isArpeggiatorEnabled() const noexcept
{
    return arpeggiatorEnabled.load();
}


void MidiSystem::timerCallback()
{
    if (! arpeggiatorEnabled.load())
    {
        stopTimer();
        return;
    }

    juce::Array<int> heldNotes;
    juce::Array<float> heldVelocities;

    {
        const juce::ScopedLock lock (
            arpeggiatorLock
        );

        for (int note = 0; note < 128; ++note)
        {
            if (! heldArpeggiatorNotes[
                    static_cast<std::size_t> (note)
                ])
            {
                continue;
            }

            heldNotes.add (note);

            heldVelocities.add (
                heldArpeggiatorVelocities[
                    static_cast<std::size_t> (note)
                ]
            );
        }
    }

    if (currentArpeggiatorNote >= 0)
    {
        sendArpeggiatorMessage (
            juce::MidiMessage::noteOff (
                1,
                currentArpeggiatorNote
            )
        );

        currentArpeggiatorNote = -1;
    }

    if (! heldNotes.isEmpty())
    {
        const auto index =
            arpeggiatorStep
            % heldNotes.size();

        currentArpeggiatorNote =
            heldNotes.getUnchecked (index);

        const auto velocity =
            heldVelocities.getUnchecked (index);

        sendArpeggiatorMessage (
            juce::MidiMessage::noteOn (
                1,
                currentArpeggiatorNote,
                velocity
            )
        );

        ++arpeggiatorStep;
    }
    else
    {
        arpeggiatorStep = 0;
    }

    const auto tempo =
        juce::jlimit (
            20.0,
            400.0,
            audioSystem.getTransportTempo()
        );

    const auto eighthNoteMilliseconds =
        juce::jmax (
            15,
            juce::roundToInt (
                30000.0 / tempo
            )
        );

    startTimer (
        eighthNoteMilliseconds
    );
}


void MidiSystem::sendArpeggiatorMessage (
    const juce::MidiMessage& message)
{
    audioSystem.handleMidiMessage (
        message
    );
}


void MidiSystem::playVirtualNote (
    int midiNote,
    float velocity)
{
    keyboardState.noteOn (
        1,
        midiNote,
        velocity
    );
}

void MidiSystem::releaseVirtualNote (
    int midiNote)
{
    keyboardState.noteOff (
        1,
        midiNote,
        0.0f
    );
}

void MidiSystem::handleIncomingMidiMessage (
    juce::MidiInput*,
    const juce::MidiMessage& message)
{
    keyboardState.processNextMidiEvent (
        message
    );

    if (
        ! message.isNoteOn()
        && ! message.isNoteOff()
    )
    {
        applyMessage (
            message
        );
    }
}

void MidiSystem::handleNoteOn (
    juce::MidiKeyboardState*,
    int midiChannel,
    int midiNoteNumber,
    float velocity)
{
    const auto message =
        juce::MidiMessage::noteOn (
            midiChannel,
            midiNoteNumber,
            static_cast<juce::uint8> (
                juce::jlimit (
                    0,
                    127,
                    juce::roundToInt (
                        velocity * 127.0f
                    )
                )
            )
        );

    applyMessage (
        message
    );
}

void MidiSystem::handleNoteOff (
    juce::MidiKeyboardState*,
    int midiChannel,
    int midiNoteNumber,
    float)
{
    const auto message =
        juce::MidiMessage::noteOff (
            midiChannel,
            midiNoteNumber
        );

    applyMessage (
        message
    );
}
void MidiSystem::applyMessage (
    const juce::MidiMessage& message)
{
    const auto isNoteOn =
        message.isNoteOn();

    const auto isNoteOff =
        message.isNoteOff();

    if (isNoteOn || isNoteOff)
    {
        const auto note =
            juce::jlimit (
                0,
                127,
                message.getNoteNumber()
            );

        {
            const juce::ScopedLock lock (
                arpeggiatorLock
            );

            heldArpeggiatorNotes[
                static_cast<std::size_t> (note)
            ] = isNoteOn;

            if (isNoteOn)
            {
                heldArpeggiatorVelocities[
                    static_cast<std::size_t> (note)
                ] = message.getFloatVelocity();
            }
        }
    }

    if (
        ! arpeggiatorEnabled.load()
        || (! isNoteOn && ! isNoteOff)
    )
    {
        audioSystem.handleMidiMessage (
            message
        );
    }

    if (isNoteOn)
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

        activeNoteCount.fetch_add (1);
        activitySerial.fetch_add (1);

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

    if (isNoteOff)
    {
        auto count =
            activeNoteCount.load();

        while (
            count > 0
            && ! activeNoteCount.compare_exchange_weak (
                count,
                count - 1
            )
        )
        {
        }

        activitySerial.fetch_add (1);

        if (activeNoteCount.load() <= 0)
        {
            lastNote.store (-1);
            lastVelocity.store (0.0f);

            audioSystem.setTestToneEnabled (
                false
            );
        }
    }
}
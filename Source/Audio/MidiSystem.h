#pragma once

#include <juce_audio_devices/juce_audio_devices.h>
#include <juce_audio_basics/juce_audio_basics.h>

#include <array>
#include <atomic>
#include <memory>
#include <vector>

class AudioSystem;

class MidiSystem :
    private juce::MidiInputCallback,
    private juce::MidiKeyboardState::Listener,
    private juce::Timer
{
public:
    explicit MidiSystem (
        AudioSystem& audioSystemToUse
    );

    ~MidiSystem() override;

    juce::MidiKeyboardState& getKeyboardState() noexcept;

    void refreshDevices();

    int getDeviceCount() const noexcept;

    juce::String getDeviceName (
        int index
    ) const;

    juce::String getDeviceIdentifier (
        int index
    ) const;

    juce::String getOpenDeviceName() const;
    juce::String getOpenDeviceIdentifier() const;
    juce::String getPreferredDeviceIdentifier() const;

    bool isPreferredDeviceAvailable() const;

    void selectComputerKeyboardOnly();

    void setAutoReconnectEnabled (
        bool shouldReconnect
    );

    bool isAutoReconnectEnabled() const noexcept;

    bool openDevice (
        int index
    );

    void closeDevice();

    bool isDeviceOpen() const noexcept;

    int getLastNote() const noexcept;
    float getLastVelocity() const noexcept;

    bool isMidiActive() const noexcept;
    int getActiveNoteCount() const noexcept;
    juce::uint32 getActivitySerial() const noexcept;

    void setArpeggiatorEnabled (
        bool shouldBeEnabled
    );

    bool isArpeggiatorEnabled() const noexcept;

    void playVirtualNote (
        int midiNote,
        float velocity
    );

    void releaseVirtualNote (
        int midiNote
    );

private:
    void timerCallback() override;

    void loadMidiPreferences();
    void saveMidiPreferences() const;

    juce::File getMidiPreferencesFile() const;

    void handleIncomingMidiMessage (
        juce::MidiInput*,
        const juce::MidiMessage&
    ) override;

    void applyMessage (
        const juce::MidiMessage&
    );

    void sendArpeggiatorMessage (
        const juce::MidiMessage&
    );

    void handleNoteOn (
        juce::MidiKeyboardState* source,
        int midiChannel,
        int midiNoteNumber,
        float velocity
    ) override;

    void handleNoteOff (
        juce::MidiKeyboardState* source,
        int midiChannel,
        int midiNoteNumber,
        float velocity
    ) override;

    AudioSystem& audioSystem;

    juce::MidiKeyboardState keyboardState;

    std::vector<
        juce::MidiDeviceInfo
    > availableInputs;

    std::unique_ptr<
        juce::MidiInput
    > openInput;

    juce::String openDeviceIdentifier;
    juce::String openDeviceName;
    juce::String preferredDeviceIdentifier;

    std::atomic<bool> autoReconnectEnabled { true };

    std::atomic<int> lastNote { -1 };
    std::atomic<float> lastVelocity { 0.0f };

    std::atomic<int> activeNoteCount { 0 };
    std::atomic<juce::uint32> activitySerial { 0 };

    std::atomic<bool> arpeggiatorEnabled { false };

    juce::CriticalSection arpeggiatorLock;

    std::array<bool, 128> heldArpeggiatorNotes {};
    std::array<float, 128> heldArpeggiatorVelocities {};

    int currentArpeggiatorNote = -1;
    int arpeggiatorStep = 0;
};

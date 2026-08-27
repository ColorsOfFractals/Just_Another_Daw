#pragma once

#include <juce_audio_devices/juce_audio_devices.h>
#include <juce_audio_basics/juce_audio_basics.h>

#include <atomic>
#include <memory>
#include <vector>

class AudioSystem;

class MidiSystem :
    private juce::MidiInputCallback
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

    bool openDevice (
        int index
    );

    void closeDevice();

    bool isDeviceOpen() const noexcept;

    int getLastNote() const noexcept;
    float getLastVelocity() const noexcept;

    void playVirtualNote (
        int midiNote,
        float velocity
    );

    void releaseVirtualNote (
        int midiNote
    );

private:
    void handleIncomingMidiMessage (
        juce::MidiInput*,
        const juce::MidiMessage&
    ) override;

    void applyMessage (
        const juce::MidiMessage&
    );

    AudioSystem& audioSystem;

    juce::MidiKeyboardState keyboardState;

    std::vector<
        juce::MidiDeviceInfo
    > availableInputs;

    std::unique_ptr<
        juce::MidiInput
    > openInput;

    std::atomic<int> lastNote { -1 };
    std::atomic<float> lastVelocity { 0.0f };
};

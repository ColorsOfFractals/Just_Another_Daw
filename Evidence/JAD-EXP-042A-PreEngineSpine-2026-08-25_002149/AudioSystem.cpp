#include "AudioSystem.h"

#include <cmath>

AudioSystem::AudioSystem()
{
    alive = true;

    const auto error =
        deviceManager.initialiseWithDefaultDevices (2, 2);

    ready =
        error.isEmpty()
        && deviceManager.getCurrentAudioDevice() != nullptr;

    if (ready)
        deviceManager.addAudioCallback (this);
}

AudioSystem::~AudioSystem()
{
    requestedGate.store (false);

    deviceManager.removeAudioCallback (this);
    deviceManager.closeAudioDevice();

    ready = false;
    alive = false;
}

bool AudioSystem::isAlive() const noexcept
{
    return alive;
}

bool AudioSystem::isReady() const noexcept
{
    return ready;
}

juce::String AudioSystem::getOutputDeviceName() const
{
    if (auto* device = deviceManager.getCurrentAudioDevice())
        return device->getName();

    return "No output device";
}

juce::String AudioSystem::getInputDeviceName() const
{
    if (auto* device = deviceManager.getCurrentAudioDevice())
        return device->getName();

    return "No input device";
}

double AudioSystem::getSampleRate() const noexcept
{
    if (auto* device = deviceManager.getCurrentAudioDevice())
        return device->getCurrentSampleRate();

    return 0.0;
}

int AudioSystem::getBufferSize() const noexcept
{
    if (auto* device = deviceManager.getCurrentAudioDevice())
        return device->getCurrentBufferSizeSamples();

    return 0;
}

void AudioSystem::setTestToneEnabled (bool enabled) noexcept
{
    requestedGate.store (enabled);
}

bool AudioSystem::isTestToneEnabled() const noexcept
{
    return requestedGate.load();
}

void AudioSystem::setFrequency (double newFrequency) noexcept
{
    frequencyHz.store (
        juce::jlimit (
            40.0,
            2000.0,
            newFrequency
        )
    );
}

double AudioSystem::getFrequency() const noexcept
{
    return frequencyHz.load();
}

void AudioSystem::setGain (float newGain) noexcept
{
    outputGain.store (
        juce::jlimit (
            0.0f,
            0.20f,
            newGain
        )
    );
}

float AudioSystem::getGain() const noexcept
{
    return outputGain.load();
}

void AudioSystem::setWaveform (Waveform newWaveform) noexcept
{
    waveform.store (
        static_cast<int> (newWaveform)
    );
}

AudioSystem::Waveform AudioSystem::getWaveform() const noexcept
{
    return static_cast<Waveform> (
        waveform.load()
    );
}

void AudioSystem::audioDeviceAboutToStart (
    juce::AudioIODevice* device)
{
    currentSampleRate =
        device != nullptr
            ? device->getCurrentSampleRate()
            : 0.0;

    phase = 0.0;
    appliedGate = false;

    envelope.reset();

    if (currentSampleRate > 0.0)
        envelope.setSampleRate (currentSampleRate);

    juce::ADSR::Parameters parameters;

    parameters.attack  = 0.08f;
    parameters.decay   = 0.22f;
    parameters.sustain = 0.65f;
    parameters.release = 0.55f;

    envelope.setParameters (parameters);
}

void AudioSystem::audioDeviceStopped()
{
    requestedGate.store (false);

    appliedGate = false;

    envelope.reset();

    currentSampleRate = 0.0;
    phase = 0.0;
}

void AudioSystem::updateEnvelopeGate() noexcept
{
    const auto requested =
        requestedGate.load();

    if (requested == appliedGate)
        return;

    if (requested)
        envelope.noteOn();

    if (! requested)
        envelope.noteOff();

    appliedGate = requested;
}

float AudioSystem::renderOscillatorSample() noexcept
{
    const auto selected =
        static_cast<Waveform> (
            waveform.load()
        );

    if (selected == Waveform::saw)
    {
        return static_cast<float> (
            (phase / juce::MathConstants<double>::pi)
            - 1.0
        );
    }

    if (selected == Waveform::square)
    {
        return phase
            < juce::MathConstants<double>::pi
                ? 1.0f
                : -1.0f;
    }

    return static_cast<float> (
        std::sin (phase)
    );
}

void AudioSystem::audioDeviceIOCallbackWithContext (
    const float* const*,
    int,
    float* const* outputChannelData,
    int numOutputChannels,
    int numSamples,
    const juce::AudioIODeviceCallbackContext&)
{
    for (int channel = 0;
         channel < numOutputChannels;
         ++channel)
    {
        if (outputChannelData[channel] != nullptr)
        {
            juce::FloatVectorOperations::clear (
                outputChannelData[channel],
                numSamples
            );
        }
    }

    if (currentSampleRate <= 0.0)
        return;

    updateEnvelopeGate();

    const auto frequency =
        frequencyHz.load();

    const auto gain =
        outputGain.load();

    const auto phaseIncrement =
        juce::MathConstants<double>::twoPi
        * frequency
        / currentSampleRate;

    for (int sample = 0;
         sample < numSamples;
         ++sample)
    {
        const auto envelopeLevel =
            envelope.getNextSample();

        const auto value =
            renderOscillatorSample()
            * gain
            * envelopeLevel;

        for (int channel = 0;
             channel < numOutputChannels;
             ++channel)
        {
            if (outputChannelData[channel] != nullptr)
            {
                outputChannelData[channel][sample] =
                    value;
            }
        }

        phase += phaseIncrement;

        while (
            phase
            >= juce::MathConstants<double>::twoPi
        )
        {
            phase -=
                juce::MathConstants<double>::twoPi;
        }
    }
}

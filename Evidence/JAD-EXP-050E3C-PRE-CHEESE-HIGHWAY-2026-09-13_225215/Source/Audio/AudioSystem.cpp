#include "AudioSystem.h"

#include "../Model/MasterBusState.h"
#include "../Model/SessionState.h"
#include "../Model/TrackModel.h"
#include "../Model/TransportState.h"

#include <algorithm>

AudioSystem::AudioSystem()
{
    alive = true;

    const auto error =
        deviceManager.initialiseWithDefaultDevices (
            2,
            2
        );

    ready =
        error.isEmpty()
        && deviceManager.getCurrentAudioDevice()
            != nullptr;

    if (ready)
        deviceManager.addAudioCallback (this);
}

AudioSystem::~AudioSystem()
{
    synthSource.setGate (false);

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
    if (
        auto* device =
            deviceManager.getCurrentAudioDevice()
    )
    {
        return device->getName();
    }

    return "No output device";
}

juce::String AudioSystem::getInputDeviceName() const
{
    if (
        auto* device =
            deviceManager.getCurrentAudioDevice()
    )
    {
        return device->getName();
    }

    return "No input device";
}

double AudioSystem::getSampleRate() const noexcept
{
    if (
        auto* device =
            deviceManager.getCurrentAudioDevice()
    )
    {
        return device->getCurrentSampleRate();
    }

    return 0.0;
}

int AudioSystem::getBufferSize() const noexcept
{
    if (
        auto* device =
            deviceManager.getCurrentAudioDevice()
    )
    {
        return device->getCurrentBufferSizeSamples();
    }

    return 0;
}

juce::AudioDeviceManager&
AudioSystem::getDeviceManager() noexcept
{
    return deviceManager;
}
void AudioSystem::setTestToneEnabled (
    bool enabled
) noexcept
{
    synthSource.setGate (
        enabled
    );
}

bool AudioSystem::isTestToneEnabled() const noexcept
{
    return synthSource.isGateEnabled();
}

void AudioSystem::setSessionState (
    SessionState* sessionStateToUse
) noexcept
{
    sessionState =
        sessionStateToUse;

    rebuildTrackRoutes();
}

void AudioSystem::setTransportState (
    TransportState* transportStateToUse
) noexcept
{
    transportState =
        transportStateToUse;
}

void AudioSystem::setMasterBusState (
    MasterBusState* masterBusStateToUse
) noexcept
{
    masterBusState =
        masterBusStateToUse;
}

void AudioSystem::setFrequency (
    double frequencyHz
) noexcept
{
    synthSource.setFrequency (
        frequencyHz
    );
}

double AudioSystem::getFrequency() const noexcept
{
    return synthSource.getFrequency();
}

void AudioSystem::setGain (
    float newGain
) noexcept
{
    synthSource.setGain (
        newGain
    );
}

float AudioSystem::getGain() const noexcept
{
    return synthSource.getGain();
}

void AudioSystem::setWaveform (
    Waveform newWaveform
) noexcept
{
    synthSource.setWaveform (
        newWaveform
    );
}

AudioSystem::Waveform
AudioSystem::getWaveform() const noexcept
{
    return synthSource.getWaveform();
}

void AudioSystem::rebuildTrackRoutes()
{
    for (
        auto& processor :
        trackProcessors
    )
    {
        processor.setModel (
            nullptr
        );

        processor.setSource (
            nullptr
        );
    }

    if (sessionState == nullptr)
        return;

    const auto count =
        std::min<std::size_t> (
            sessionState->getTrackCount(),
            trackProcessors.size()
        );

    for (
        std::size_t index = 0;
        index < count;
        ++index
    )
    {
        trackProcessors[index].setModel (
            sessionState->getTrack (index)
        );
    }

    // First real source enters the track fleet.
    trackProcessors[0].setSource (
        &synthSource
    );

    if (
        currentSampleRate > 0.0
        && currentBlockSize > 0
    )
    {
        for (
            auto& processor :
            trackProcessors
        )
        {
            processor.prepare (
                currentSampleRate,
                currentBlockSize
            );
        }
    }
}

void AudioSystem::audioDeviceAboutToStart (
    juce::AudioIODevice* device
)
{
    currentSampleRate =
        device != nullptr
            ? device->getCurrentSampleRate()
            : 0.0;

    currentBlockSize =
        device != nullptr
            ? device->getCurrentBufferSizeSamples()
            : 0;

    masterMixBuffer.setSize (
        2,
        std::max (
            1,
            currentBlockSize
        ),
        false,
        true,
        true
    );

    masterMixBuffer.clear();

    synthSource.prepare (
        currentSampleRate,
        currentBlockSize
    );

    rebuildTrackRoutes();
}

void AudioSystem::audioDeviceStopped()
{
    synthSource.setGate (false);
    synthSource.reset();

    for (
        auto& processor :
        trackProcessors
    )
    {
        processor.reset();
    }

    masterMixBuffer.clear();

    currentSampleRate = 0.0;
    currentBlockSize = 0;
}

void AudioSystem::audioDeviceIOCallbackWithContext (
    const float* const*,
    int,
    float* const* outputChannelData,
    int numOutputChannels,
    int numSamples,
    const juce::AudioIODeviceCallbackContext&
)
{
    for (
        int channel = 0;
        channel < numOutputChannels;
        ++channel
    )
    {
        if (
            outputChannelData[channel]
            != nullptr
        )
        {
            juce::FloatVectorOperations::clear (
                outputChannelData[channel],
                numSamples
            );
        }
    }

    if (currentSampleRate <= 0.0)
        return;

    if (numSamples <= 0)
        return;

    if (
        masterMixBuffer.getNumSamples()
        < numSamples
    )
    {
        masterMixBuffer.setSize (
            2,
            numSamples,
            false,
            false,
            true
        );
    }

    masterMixBuffer.clear();

    if (transportState != nullptr)
    {
        transportState->advanceSamples (
            numSamples,
            currentSampleRate
        );
    }

    bool anyTrackSoloed = false;

    if (sessionState != nullptr)
    {
        const auto count =
            std::min<std::size_t> (
                sessionState->getTrackCount(),
                trackProcessors.size()
            );

        for (
            std::size_t index = 0;
            index < count;
            ++index
        )
        {
            const auto* track =
                sessionState->getTrack (
                    index
                );

            if (
                track != nullptr
                && track->isSolo()
            )
            {
                anyTrackSoloed = true;
                break;
            }
        }
    }

    for (
        auto& processor :
        trackProcessors
    )
    {
        processor.process (
            masterMixBuffer,
            numSamples,
            anyTrackSoloed
        );
    }

    const auto masterGain =
        masterBusState != nullptr
            ? masterBusState->getGain()
            : 1.0f;

    const auto masterMuted =
        masterBusState != nullptr
            ? masterBusState->isMuted()
            : false;

    const auto effectiveMasterGain =
        masterMuted
            ? 0.0f
            : masterGain;

    masterMixBuffer.applyGain (
        effectiveMasterGain
    );

    const auto channelsToCopy =
        std::min (
            numOutputChannels,
            masterMixBuffer.getNumChannels()
        );

    for (
        int channel = 0;
        channel < channelsToCopy;
        ++channel
    )
    {
        if (
            outputChannelData[channel]
            == nullptr
        )
        {
            continue;
        }

        juce::FloatVectorOperations::copy (
            outputChannelData[channel],
            masterMixBuffer.getReadPointer (
                channel
            ),
            numSamples
        );
    }
}


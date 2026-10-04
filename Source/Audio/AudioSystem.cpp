#include "AudioSystem.h"

#include "../Model/MasterBusState.h"
#include "../Model/SessionState.h"
#include "../Model/TrackModel.h"
#include "../Model/TransportState.h"

#include <algorithm>
#include <cmath>

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


double AudioSystem::getTransportTempo() const noexcept
{
    if (transportState == nullptr)
        return 120.0;

    return transportState->getTempo();
}


juce::AudioDeviceManager&
AudioSystem::getDeviceManager() noexcept
{
    return deviceManager;
}


void AudioSystem::setMidiTargetTrack (
    int trackIndex) noexcept
{
    midiTargetTrackIndex.store (
        juce::jlimit (0, maxTrackCount - 1, trackIndex)
    );
}


int AudioSystem::getMidiTargetTrack() const noexcept
{
    return midiTargetTrackIndex.load();
}


void AudioSystem::handleMidiMessage (
    const juce::MidiMessage& message)
{
    const auto trackIndex =
        midiTargetTrackIndex.load();

    const auto isCapturableMessage =
        message.isNoteOn()
        || message.isNoteOff();

    if (
        isCapturableMessage
        && sessionState != nullptr
        && transportState != nullptr
        && transportState->isRecording()
        && trackIndex >= 0
        && trackIndex < maxTrackCount
    )
    {
        if (
            auto* track = sessionState->getTrack (
                static_cast<std::size_t> (trackIndex)
            );
            track != nullptr
            && track->isRecordArmed()
        )
        {
            if (! recordingCaptureActive.exchange (true))
                track->clearRecordedMidiEvents();

            track->recordMidiEvent (
                message.isNoteOn(),
                message.getNoteNumber(),
                message.getFloatVelocity(),
                message.getChannel(),
                transportState->getPositionInBeats()
            );
        }
    }

    if (
        trackIndex >= 0
        && trackIndex < maxTrackCount
        && pluginSources[trackIndex].hasPlugin()
    )
    {
        pluginSources[trackIndex].addMidiMessage (
            message
        );
    }
}


bool AudioSystem::installTrackPlugin (
    int trackIndex,
    std::unique_ptr<juce::AudioPluginInstance> instance)
{
    if (
        instance == nullptr
        || trackIndex < 0
        || trackIndex >= maxTrackCount
        || currentSampleRate <= 0.0
        || currentBlockSize <= 0
    )
    {
        return false;
    }

    const juce::ScopedLock callbackLock (
        deviceManager.getAudioCallbackLock()
    );

    auto& source =
        pluginSources[trackIndex];

    source.prepare (
        currentSampleRate,
        currentBlockSize
    );

    source.setPlugin (
        std::move (instance)
    );

    midiTargetTrackIndex.store (trackIndex);
    rebuildTrackRoutes();

    return source.hasPlugin();
}


void AudioSystem::restoreNativeSynth (
    int trackIndex)
{
    if (
        trackIndex < 0
        || trackIndex >= maxTrackCount
    )
    {
        return;
    }

    const juce::ScopedLock callbackLock (
        deviceManager.getAudioCallbackLock()
    );

    pluginSources[trackIndex].clearPlugin();
    rebuildTrackRoutes();
}


bool AudioSystem::hasTrackPlugin (
    int trackIndex) const noexcept
{
    return trackIndex >= 0
        && trackIndex < maxTrackCount
        && pluginSources[trackIndex].hasPlugin();
}


juce::String AudioSystem::getTrackPluginName (
    int trackIndex) const
{
    if (! hasTrackPlugin (trackIndex))
        return {};

    return pluginSources[trackIndex].getPluginName();
}


juce::AudioPluginInstance*
AudioSystem::getTrackPlugin (
    int trackIndex) noexcept
{
    if (! hasTrackPlugin (trackIndex))
        return nullptr;

    return pluginSources[trackIndex].getPlugin();
}


void AudioSystem::refreshTrackRoutes()
{
    const juce::ScopedLock callbackLock (
        deviceManager.getAudioCallbackLock()
    );

    rebuildTrackRoutes();
}


void AudioSystem::handleTrackRemoved (
    int removedTrackIndex)
{
    if (
        removedTrackIndex < 0
        || removedTrackIndex >= maxTrackCount
    )
    {
        refreshTrackRoutes();
        return;
    }

    const juce::ScopedLock callbackLock (
        deviceManager.getAudioCallbackLock()
    );

    // Slot identity is index-based in EXP-051A. Clearing the removed slot and
    // every later slot prevents a plugin from silently jumping tracks after
    // SessionState compacts its vector. Later persistence work can key slots
    // by TrackId without risking a wrong-route surprise today.
    for (
        int index = removedTrackIndex;
        index < maxTrackCount;
        ++index
    )
    {
        pluginSources[index].clearPlugin();
    }

    const auto remainingTrackCount =
        sessionState != nullptr
            ? static_cast<int> (sessionState->getTrackCount())
            : 0;

    midiTargetTrackIndex.store (
        remainingTrackCount > 0
            ? juce::jmin (
                removedTrackIndex,
                remainingTrackCount - 1
            )
            : 0
    );

    rebuildTrackRoutes();
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

    for (
        std::size_t index = 0;
        index < count;
        ++index
    )
    {
        auto* source =
            pluginSources[index].hasPlugin()
                ? static_cast<AudioSource*> (&pluginSources[index])
                : nullptr;

        // Track 1 keeps the native synth as the recoverable lifeboat until an
        // actual plugin is assigned there.
        if (index == 0 && source == nullptr)
            source = static_cast<AudioSource*> (&synthSource);

        trackProcessors[index].setSource (source);
    }

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

    for (auto& source : pluginSources)
    {
        source.prepare (
            currentSampleRate,
            currentBlockSize
        );
    }

    rebuildTrackRoutes();
}

void AudioSystem::audioDeviceStopped()
{
    synthSource.setGate (false);
    synthSource.reset();
    for (auto& source : pluginSources)
        source.reset();

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

void AudioSystem::triggerMetronomeClick (
    bool accented) noexcept
{
    if (
        transportState == nullptr
        || currentSampleRate <= 0.0
    )
    {
        return;
    }

    const auto sound =
        transportState->getMetronomeSound();

    const auto accentMultiplier =
        accented
            ? 1.0f
            : 0.72f;

    metronomePhase = 0.0;

    switch (sound)
    {
        case 1:
            metronomeFrequency =
                accented
                    ? 940.0
                    : 690.0;

            metronomeClickLengthSamples =
                static_cast<int> (
                    currentSampleRate
                    * 0.042
                );
            break;

        case 2:
            metronomeFrequency =
                accented
                    ? 1480.0
                    : 1040.0;

            metronomeClickLengthSamples =
                static_cast<int> (
                    currentSampleRate
                    * 0.068
                );
            break;

        case 3:
            metronomeFrequency =
                accented
                    ? 2650.0
                    : 1830.0;

            metronomeClickLengthSamples =
                static_cast<int> (
                    currentSampleRate
                    * 0.050
                );
            break;

        case 0:
        default:
            metronomeFrequency =
                accented
                    ? 2180.0
                    : 1510.0;

            metronomeClickLengthSamples =
                static_cast<int> (
                    currentSampleRate
                    * 0.028
                );
            break;
    }

    metronomeClickLengthSamples =
        juce::jmax (
            1,
            metronomeClickLengthSamples
        );

    metronomeSamplesRemaining =
        metronomeClickLengthSamples;

    metronomeClickAmplitude =
        transportState->getMetronomeVolume()
        * accentMultiplier;
}


void AudioSystem::renderMetronome (
    juce::AudioBuffer<float>& destination,
    int numSamples,
    double blockStartBeat) noexcept
{
    if (
        transportState == nullptr
        || currentSampleRate <= 0.0
        || numSamples <= 0
    )
    {
        return;
    }

    const auto recordingNow =
        transportState->isRecording();

    const auto clickAllowed =
        transportState->isMetronomeEnabled()
        && transportState->isPlaying()
        && (
            recordingNow
                ? transportState
                    ->isClickDuringRecordingEnabled()
                : transportState
                    ->isClickDuringPlaybackEnabled()
        );

    if (! clickAllowed)
    {
        lastMetronomeBeat = -1;
        metronomeSamplesRemaining = 0;
        return;
    }

    const auto tempo =
        transportState->getTempo();

    if (tempo <= 0.0)
        return;

    const auto beatsPerSample =
        tempo
        / 60.0
        / currentSampleRate;

    const auto numerator =
        juce::jmax (
            1,
            transportState
                ->getTimeSignatureNumerator()
        );

    if (lastMetronomeBeat < 0)
    {
        const auto startWholeBeat =
            static_cast<std::int64_t> (
                std::floor (
                    blockStartBeat
                )
            );

        const auto fraction =
            blockStartBeat
            - std::floor (
                blockStartBeat
            );

        if (
            fraction
            > beatsPerSample * 2.0
        )
        {
            lastMetronomeBeat =
                startWholeBeat;
        }
    }

    const auto sound =
        transportState->getMetronomeSound();

    const auto channels =
        destination.getNumChannels();

    for (
        int sample = 0;
        sample < numSamples;
        ++sample
    )
    {
        const auto beatPosition =
            blockStartBeat
            + static_cast<double> (
                sample
            )
            * beatsPerSample;

        const auto wholeBeat =
            static_cast<std::int64_t> (
                std::floor (
                    beatPosition
                    + 1.0e-9
                )
            );

        if (
            wholeBeat
            != lastMetronomeBeat
        )
        {
            const auto beatInBar =
                static_cast<int> (
                    (
                        wholeBeat
                        % numerator
                        + numerator
                    )
                    % numerator
                );

            const auto accented =
                transportState
                    ->isAccentFirstBeatEnabled()
                && beatInBar == 0;

            triggerMetronomeClick (
                accented
            );

            lastMetronomeBeat =
                wholeBeat;
        }

        if (
            metronomeSamplesRemaining
            <= 0
        )
        {
            continue;
        }

        const auto envelope =
            static_cast<float> (
                metronomeSamplesRemaining
            )
            / static_cast<float> (
                metronomeClickLengthSamples
            );

        const auto shapedEnvelope =
            envelope * envelope;

        const auto sine =
            static_cast<float> (
                std::sin (
                    metronomePhase
                )
            );

        float voice = sine;

        if (sound == 1)
        {
            metronomeNoiseState ^=
                metronomeNoiseState << 13;

            metronomeNoiseState ^=
                metronomeNoiseState >> 17;

            metronomeNoiseState ^=
                metronomeNoiseState << 5;

            const auto noise =
                static_cast<float> (
                    metronomeNoiseState
                    & 0xffffu
                )
                / 32767.5f
                - 1.0f;

            voice =
                noise * 0.74f
                + sine * 0.26f;
        }
        else if (sound == 2)
        {
            voice =
                sine
                + 0.28f
                    * static_cast<float> (
                        std::sin (
                            metronomePhase * 2.0
                        )
                    );
        }
        else if (sound == 3)
        {
            voice =
                sine >= 0.0f
                    ? 0.82f
                    : -0.82f;

            voice +=
                0.24f
                * static_cast<float> (
                    std::sin (
                        metronomePhase * 2.0
                    )
                );
        }

        const auto value =
            voice
            * shapedEnvelope
            * metronomeClickAmplitude
            * 0.34f;

        for (
            int channel = 0;
            channel < channels;
            ++channel
        )
        {
            destination.addSample (
                channel,
                sample,
                value
            );
        }

        metronomePhase +=
            juce::MathConstants<double>::twoPi
            * metronomeFrequency
            / currentSampleRate;

        while (
            metronomePhase
            >= juce::MathConstants<double>::twoPi
        )
        {
            metronomePhase -=
                juce::MathConstants<double>::twoPi;
        }

        --metronomeSamplesRemaining;
    }
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

    if (
        transportState == nullptr
        || ! transportState->isRecording()
    )
    {
        recordingCaptureActive.store (false);
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

    const auto metronomeBlockStartBeat =
        transportState != nullptr
            ? transportState
                ->getPositionInBeats()
            : 0.0;

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

    renderMetronome (
        masterMixBuffer,
        numSamples,
        metronomeBlockStartBeat
    );

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


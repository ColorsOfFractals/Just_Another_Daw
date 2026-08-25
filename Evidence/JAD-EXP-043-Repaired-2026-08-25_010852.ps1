& {
    $ErrorActionPreference = 'Stop'

    $R = "$HOME\Tinkering\Just_Another_Daw"
    Set-Location $R

    $Stamp = Get-Date -Format 'yyyy-MM-dd_HHmmss'
    $Evidence = "$R\Evidence"
    $Backup = "$Evidence\JAD-EXP-043-PreAudioHighway-$Stamp"

    New-Item -ItemType Directory -Force -Path $Backup | Out-Null

    Write-Host ""
    Write-Host "============================================================" -ForegroundColor DarkRed
    Write-Host " JAD — EXPEDITION #043" -ForegroundColor Red
    Write-Host " AUDIO HIGHWAY" -ForegroundColor Red
    Write-Host "============================================================" -ForegroundColor DarkRed
    Write-Host ""

    Write-Host "[Camp] Fossilizing the living 042A-R3 organism..." -ForegroundColor Yellow

    Copy-Item "$R\Source" "$Backup\Source" -Recurse
    Copy-Item "$R\CMakeLists.txt" "$Backup\CMakeLists.txt"

    # ------------------------------------------------------------
    # AUDIO SOURCE CONTRACT
    # ------------------------------------------------------------

    $AudioSourceH = @'
#pragma once

#include <juce_audio_basics/juce_audio_basics.h>

class AudioSource
{
public:
    virtual ~AudioSource() = default;

    virtual void prepare (
        double sampleRate,
        int maximumBlockSize
    ) = 0;

    virtual void reset() = 0;

    virtual void render (
        juce::AudioBuffer<float>& destination,
        int numSamples
    ) = 0;
};
'@

    # ------------------------------------------------------------
    # TRACK PROCESSOR
    # ------------------------------------------------------------

    $TrackProcessorH = @'
#pragma once

#include "AudioSource.h"

#include <juce_audio_basics/juce_audio_basics.h>

class TrackModel;

class TrackProcessor
{
public:
    TrackProcessor() = default;

    void setModel (
        TrackModel* modelToUse
    ) noexcept;

    void setSource (
        AudioSource* sourceToUse
    ) noexcept;

    void prepare (
        double sampleRate,
        int maximumBlockSize
    );

    void reset();

    void process (
        juce::AudioBuffer<float>& mixBuffer,
        int numSamples,
        bool anyTrackSoloed
    );

private:
    TrackModel* model = nullptr;
    AudioSource* source = nullptr;

    juce::AudioBuffer<float> workingBuffer;
};
'@

    $TrackProcessorCpp = @'
#include "TrackProcessor.h"

#include "../Model/TrackModel.h"

#include <algorithm>
#include <cmath>

void TrackProcessor::setModel (
    TrackModel* modelToUse
) noexcept
{
    model = modelToUse;
}

void TrackProcessor::setSource (
    AudioSource* sourceToUse
) noexcept
{
    source = sourceToUse;
}

void TrackProcessor::prepare (
    double sampleRate,
    int maximumBlockSize
)
{
    workingBuffer.setSize (
        2,
        std::max (1, maximumBlockSize),
        false,
        true,
        true
    );

    workingBuffer.clear();

    if (source != nullptr)
    {
        source->prepare (
            sampleRate,
            maximumBlockSize
        );
    }
}

void TrackProcessor::reset()
{
    workingBuffer.clear();

    if (source != nullptr)
        source->reset();
}

void TrackProcessor::process (
    juce::AudioBuffer<float>& mixBuffer,
    int numSamples,
    bool anyTrackSoloed
)
{
    if (model == nullptr)
        return;

    if (source == nullptr)
        return;

    if (numSamples <= 0)
        return;

    const auto muted =
        model->isMuted();

    const auto soloed =
        model->isSolo();

    if (muted)
        return;

    if (anyTrackSoloed && ! soloed)
        return;

    if (
        workingBuffer.getNumSamples()
        < numSamples
    )
    {
        workingBuffer.setSize (
            2,
            numSamples,
            false,
            false,
            true
        );
    }

    workingBuffer.clear();

    source->render (
        workingBuffer,
        numSamples
    );

    const auto gain =
        model->getGain();

    const auto pan =
        model->getPan();

    const auto angle =
        static_cast<float> (
            (pan + 1.0f)
            * (juce::MathConstants<float>::pi * 0.25f)
        );

    const auto leftGain =
        gain * std::cos (angle);

    const auto rightGain =
        gain * std::sin (angle);

    workingBuffer.applyGain (
        0,
        0,
        numSamples,
        leftGain
    );

    workingBuffer.applyGain (
        1,
        0,
        numSamples,
        rightGain
    );

    const auto channels =
        std::min (
            2,
            mixBuffer.getNumChannels()
        );

    for (
        int channel = 0;
        channel < channels;
        ++channel
    )
    {
        mixBuffer.addFrom (
            channel,
            0,
            workingBuffer,
            channel,
            0,
            numSamples
        );
    }
}
'@

    # ------------------------------------------------------------
    # SYNTH SOURCE
    # Existing oscillator becomes a source instead of the mixer.
    # ------------------------------------------------------------

    $SynthSourceH = @'
#pragma once

#include "AudioSource.h"

#include <juce_audio_basics/juce_audio_basics.h>

#include <atomic>

class SynthSource : public AudioSource
{
public:
    enum class Waveform
    {
        sine = 0,
        saw,
        square
    };

    void prepare (
        double sampleRate,
        int maximumBlockSize
    ) override;

    void reset() override;

    void render (
        juce::AudioBuffer<float>& destination,
        int numSamples
    ) override;

    void setGate (
        bool enabled
    ) noexcept;

    bool isGateEnabled() const noexcept;

    void setFrequency (
        double frequencyHz
    ) noexcept;

    double getFrequency() const noexcept;

    void setGain (
        float newGain
    ) noexcept;

    float getGain() const noexcept;

    void setWaveform (
        Waveform newWaveform
    ) noexcept;

    Waveform getWaveform() const noexcept;

private:
    void updateEnvelopeGate() noexcept;
    float renderOscillatorSample() noexcept;

    juce::ADSR envelope;

    std::atomic<bool> requestedGate { false };

    std::atomic<double> frequencyHz { 440.0 };
    std::atomic<float> outputGain { 0.05f };

    std::atomic<int> waveform {
        static_cast<int> (Waveform::sine)
    };

    double currentSampleRate = 0.0;
    double phase = 0.0;

    bool appliedGate = false;
};
'@

    $SynthSourceCpp = @'
#include "SynthSource.h"

#include <cmath>

void SynthSource::prepare (
    double sampleRate,
    int
)
{
    currentSampleRate = sampleRate;

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

    envelope.setParameters (
        parameters
    );
}

void SynthSource::reset()
{
    requestedGate.store (false);

    appliedGate = false;

    envelope.reset();

    phase = 0.0;
}

void SynthSource::setGate (
    bool enabled
) noexcept
{
    requestedGate.store (
        enabled
    );
}

bool SynthSource::isGateEnabled() const noexcept
{
    return requestedGate.load();
}

void SynthSource::setFrequency (
    double newFrequency
) noexcept
{
    frequencyHz.store (
        juce::jlimit (
            40.0,
            2000.0,
            newFrequency
        )
    );
}

double SynthSource::getFrequency() const noexcept
{
    return frequencyHz.load();
}

void SynthSource::setGain (
    float newGain
) noexcept
{
    outputGain.store (
        juce::jlimit (
            0.0f,
            0.20f,
            newGain
        )
    );
}

float SynthSource::getGain() const noexcept
{
    return outputGain.load();
}

void SynthSource::setWaveform (
    Waveform newWaveform
) noexcept
{
    waveform.store (
        static_cast<int> (
            newWaveform
        )
    );
}

SynthSource::Waveform
SynthSource::getWaveform() const noexcept
{
    return static_cast<Waveform> (
        waveform.load()
    );
}

void SynthSource::updateEnvelopeGate() noexcept
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

float SynthSource::renderOscillatorSample() noexcept
{
    const auto selected =
        static_cast<Waveform> (
            waveform.load()
        );

    if (selected == Waveform::saw)
    {
        return static_cast<float> (
            (phase
                / juce::MathConstants<double>::pi)
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

void SynthSource::render (
    juce::AudioBuffer<float>& destination,
    int numSamples
)
{
    if (currentSampleRate <= 0.0)
        return;

    if (numSamples <= 0)
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

    const auto channels =
        std::min (
            2,
            destination.getNumChannels()
        );

    for (
        int sample = 0;
        sample < numSamples;
        ++sample
    )
    {
        const auto envelopeLevel =
            envelope.getNextSample();

        const auto value =
            renderOscillatorSample()
            * gain
            * envelopeLevel;

        for (
            int channel = 0;
            channel < channels;
            ++channel
        )
        {
            destination.setSample (
                channel,
                sample,
                value
            );
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
'@

    # ------------------------------------------------------------
    # NEW AUDIO SYSTEM
    # ------------------------------------------------------------

    $AudioSystemH = @'
#pragma once

#include "SynthSource.h"
#include "TrackProcessor.h"

#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_audio_devices/juce_audio_devices.h>

#include <array>

class SessionState;
class TransportState;
class MasterBusState;

class AudioSystem : private juce::AudioIODeviceCallback
{
public:
    using Waveform = SynthSource::Waveform;

    AudioSystem();
    ~AudioSystem() override;

    bool isAlive() const noexcept;
    bool isReady() const noexcept;

    juce::String getOutputDeviceName() const;
    juce::String getInputDeviceName() const;

    double getSampleRate() const noexcept;
    int getBufferSize() const noexcept;

    void setTestToneEnabled (
        bool enabled
    ) noexcept;

    bool isTestToneEnabled() const noexcept;

    void setSessionState (
        SessionState* sessionStateToUse
    ) noexcept;

    void setTransportState (
        TransportState* transportStateToUse
    ) noexcept;

    void setMasterBusState (
        MasterBusState* masterBusStateToUse
    ) noexcept;

    void setFrequency (
        double frequencyHz
    ) noexcept;

    double getFrequency() const noexcept;

    void setGain (
        float newGain
    ) noexcept;

    float getGain() const noexcept;

    void setWaveform (
        Waveform newWaveform
    ) noexcept;

    Waveform getWaveform() const noexcept;

private:
    void audioDeviceIOCallbackWithContext (
        const float* const* inputChannelData,
        int numInputChannels,
        float* const* outputChannelData,
        int numOutputChannels,
        int numSamples,
        const juce::AudioIODeviceCallbackContext& context
    ) override;

    void audioDeviceAboutToStart (
        juce::AudioIODevice* device
    ) override;

    void audioDeviceStopped() override;

    void rebuildTrackRoutes();

    SessionState* sessionState = nullptr;
    TransportState* transportState = nullptr;
    MasterBusState* masterBusState = nullptr;

    juce::AudioDeviceManager deviceManager;

    SynthSource synthSource;

    std::array<
        TrackProcessor,
        4
    > trackProcessors;

    juce::AudioBuffer<float> masterMixBuffer;

    double currentSampleRate = 0.0;
    int currentBlockSize = 0;

    bool alive = false;
    bool ready = false;
};
'@

    $AudioSystemCpp = @'
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
'@

    # ------------------------------------------------------------
    # WRITE ROOMS
    # ------------------------------------------------------------

    Write-Host "[1/6] Pouring AudioSource contract..." -ForegroundColor Cyan

    New-Item `
        -ItemType Directory `
        -Force `
        -Path "$R\Source\Audio" |
        Out-Null

    Set-Content `
        "$R\Source\Audio\AudioSource.h" `
        $AudioSourceH `
        -Encoding UTF8

    Write-Host "[2/6] Forging TrackProcessor..." -ForegroundColor Cyan

    Set-Content `
        "$R\Source\Audio\TrackProcessor.h" `
        $TrackProcessorH `
        -Encoding UTF8

    Set-Content `
        "$R\Source\Audio\TrackProcessor.cpp" `
        $TrackProcessorCpp `
        -Encoding UTF8

    Write-Host "[3/6] Extracting synth into a source..." -ForegroundColor Cyan

    Set-Content `
        "$R\Source\Audio\SynthSource.h" `
        $SynthSourceH `
        -Encoding UTF8

    Set-Content `
        "$R\Source\Audio\SynthSource.cpp" `
        $SynthSourceCpp `
        -Encoding UTF8

    Write-Host "[4/6] Replacing direct oscillator road with highway..." -ForegroundColor Cyan

    Set-Content `
        "$R\Source\Audio\AudioSystem.h" `
        $AudioSystemH `
        -Encoding UTF8

    Set-Content `
        "$R\Source\Audio\AudioSystem.cpp" `
        $AudioSystemCpp `
        -Encoding UTF8

    # ------------------------------------------------------------
    # JAD CONTEXT — CONNECT WHOLE SESSION
    # ------------------------------------------------------------

    Write-Host "[5/6] Plugging SessionState into AudioSystem..." -ForegroundColor Cyan

    $ContextPath =
        "$R\Source\Core\JADContext.cpp"

    $Context =
        Get-Content $ContextPath -Raw

    if (
        $Context -notmatch
        'setSessionState'
    )
    {
        $Needle = @'
JADContext::JADContext()
{
'@

        $Replacement = @'
JADContext::JADContext()
{
    audioSystem.setSessionState (
        &sessionState
    );

'@

        $Context =
            $Context.Replace(
                $Needle,
                $Replacement
            )

        Set-Content `
            $ContextPath `
            $Context `
            -Encoding UTF8
    }

    # ------------------------------------------------------------
    # CMAKE — INSERT NEW AUDIO ROOMS
    # ------------------------------------------------------------

    Write-Host "[6/6] Teaching CMake the new rooms..." -ForegroundColor Cyan

    $CMakePath =
        "$R\CMakeLists.txt"

    $CMake =
        Get-Content $CMakePath -Raw

    if (
        $CMake -notmatch
        'Source/Audio/TrackProcessor.cpp'
    )
    {
        $Needle =
            '        Source/Audio/AudioSystem.cpp'

        $Replacement = @'
        Source/Audio/AudioSystem.cpp
        Source/Audio/AudioSource.h
        Source/Audio/SynthSource.cpp
        Source/Audio/SynthSource.h
        Source/Audio/TrackProcessor.cpp
        Source/Audio/TrackProcessor.h
'@

        $CMake =
            $CMake.Replace(
                $Needle,
                $Replacement.TrimEnd()
            )

        Set-Content `
            $CMakePath `
            $CMake `
            -Encoding UTF8
    }

    # ------------------------------------------------------------
    # CONTRACT GATES
    # ------------------------------------------------------------

    Write-Host ""
    Write-Host "=== CONTRACT GATES ===" -ForegroundColor Magenta

    $Required = @(
        "$R\Source\Audio\AudioSource.h"
        "$R\Source\Audio\SynthSource.h"
        "$R\Source\Audio\SynthSource.cpp"
        "$R\Source\Audio\TrackProcessor.h"
        "$R\Source\Audio\TrackProcessor.cpp"
    )

    foreach ($Path in $Required) {
        if (-not (Test-Path $Path)) {
            throw "Missing audio-highway room: $Path"
        }

        Write-Host " [+] $($Path.Replace("$R\",''))" -ForegroundColor Green
    }

    $Checks = @(
        @{
            Path = "$R\Source\Audio\AudioSystem.cpp"
            Pattern = 'trackProcessors'
            Name = 'Track fleet enters callback'
        }
        @{
            Path = "$R\Source\Audio\TrackProcessor.cpp"
            Pattern = 'isMuted'
            Name = 'Mute enters audio path'
        }
        @{
            Path = "$R\Source\Audio\TrackProcessor.cpp"
            Pattern = 'isSolo'
            Name = 'Solo enters audio path'
        }
        @{
            Path = "$R\Source\Audio\TrackProcessor.cpp"
            Pattern = 'getPan'
            Name = 'Pan enters audio path'
        }
        @{
            Path = "$R\Source\Audio\TrackProcessor.cpp"
            Pattern = 'getGain'
            Name = 'Track gain enters audio path'
        }
        @{
            Path = "$R\Source\Audio\AudioSystem.cpp"
            Pattern = 'masterMixBuffer'
            Name = 'Master summing buffer exists'
        }
        @{
            Path = "$R\Source\Core\JADContext.cpp"
            Pattern = 'setSessionState'
            Name = 'Session wired to engine'
        }
    )

    foreach ($Check in $Checks) {
        $Found =
            Select-String `
                -Path $Check.Path `
                -Pattern $Check.Pattern `
                -Quiet

        if (-not $Found) {
            throw "Contract failed: $($Check.Name)"
        }

        Write-Host " [+] $($Check.Name)" -ForegroundColor Green
    }

    Write-Host ""
    Write-Host "              🎹" -ForegroundColor Magenta
    Write-Host "              │"
    Write-Host "         SynthSource" -ForegroundColor Cyan
    Write-Host "              │"
    Write-Host "              ▼"
    Write-Host "       ┌─────────────┐"
    Write-Host "       │   TRACK 1   │" -ForegroundColor Cyan
    Write-Host "       │ GAIN / PAN  │"
    Write-Host "       │ MUTE / SOLO │"
    Write-Host "       └──────┬──────┘"
    Write-Host "              │"
    Write-Host "   TRACK 2 ───┤"
    Write-Host "   TRACK 3 ───┼──► SUM BUS" -ForegroundColor Yellow
    Write-Host "   TRACK 4 ───┤"
    Write-Host "              │"
    Write-Host "              ▼"
    Write-Host "         MASTER BUS" -ForegroundColor Red
    Write-Host "              │"
    Write-Host "              ▼"
    Write-Host "             🎧"
    Write-Host ""
    Write-Host "SHONK HAS ENTERED THE MIXER." -ForegroundColor Magenta

    # ------------------------------------------------------------
    # BUILD
    # ------------------------------------------------------------

    $BuildDir =
        "$R\Build\JUCE-Foundation"

    $VsWhere =
        "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe"

    if (-not (Test-Path $VsWhere)) {
        throw "vswhere.exe missing."
    }

    $VS =
        & $VsWhere `
            -latest `
            -products * `
            -requires Microsoft.Component.MSBuild `
            -property installationPath

    if (-not $VS) {
        throw "Visual Studio installation not found."
    }

    $DevCmd =
        Join-Path $VS `
            'Common7\Tools\VsDevCmd.bat'

    if (-not (Test-Path $DevCmd)) {
        throw "VsDevCmd.bat missing: $DevCmd"
    }

    $BuildScript =
        "$Evidence\JAD-Audio-Highway-$Stamp.cmd"

    @"
@echo off
call "$DevCmd" -arch=x64 -host_arch=x64
if errorlevel 1 exit /b %errorlevel%

cd /d "$R"

echo ============================================================
echo JAD EXP-043 - AUDIO HIGHWAY
echo ============================================================

echo.
echo [1/2] DRAWING THE ROAD
cmake -S "$R" -B "$BuildDir"
if errorlevel 1 exit /b %errorlevel%

echo.
echo [2/2] DRIVING THE SHONK
cmake --build "$BuildDir" --config Debug
exit /b %errorlevel%
"@ | Set-Content `
        $BuildScript `
        -Encoding ASCII

    Write-Host ""
    Write-Host "[Forge] Compiling the entire audio highway..." -ForegroundColor Yellow
    Write-Host ""

    & cmd.exe /d /c "`"$BuildScript`""
    $BuildExit = $LASTEXITCODE

    if ($BuildExit -ne 0) {
        Write-Host ""
        Write-Host "        🦈" -ForegroundColor Red
        Write-Host "        │"
        Write-Host "   ─────┴─────"
        Write-Host "    ROAD CLOSED" -ForegroundColor Red
        Write-Host ""
        Write-Host "Shonk found a pothole." -ForegroundColor Yellow
        Write-Host "Backup: $Backup" -ForegroundColor DarkGray
        Write-Host ""
        throw "EXP-043 build failed with exit code $BuildExit"
    }

    $Exe =
        "$BuildDir\JAD_artefacts\Debug\JAD.exe"

    if (-not (Test-Path $Exe)) {
        throw "Build succeeded but JAD.exe was not found."
    }

    $Hash =
        (Get-FileHash `
            $Exe `
            -Algorithm SHA256).Hash

    Write-Host ""
    Write-Host "============================================================" -ForegroundColor Green
    Write-Host " AUDIO HIGHWAY — OPEN" -ForegroundColor Green
    Write-Host "============================================================" -ForegroundColor Green
    Write-Host ""
    Write-Host "Track 1 now owns the synth source." -ForegroundColor Cyan
    Write-Host "Tracks 2-4 are real mixer lanes awaiting sources." -ForegroundColor Cyan
    Write-Host "Master bus sits after the track sum." -ForegroundColor Cyan
    Write-Host ""
    Write-Host "Flight test:" -ForegroundColor Yellow
    Write-Host "  1. Turn VOICE on."
    Write-Host "  2. Confirm the synth still speaks."
    Write-Host "  3. PLAY transport."
    Write-Host "  4. Confirm position still advances."
    Write-Host "  5. Close JAD normally."
    Write-Host ""
    Write-Host "ExeSHA: $Hash" -ForegroundColor DarkGray
    Write-Host ""
    Write-Host "Launching JAD..." -ForegroundColor Magenta

    & $Exe

    $RuntimeExit =
        $LASTEXITCODE

    $Rock = @"
╔════════════════════════════════════════════════════════════╗
║                   JAD AUDIO HIGHWAY                      ║
╠════════════════════════════════════════════════════════════╣
║ Expedition : JAD-EXP-043
║ State      : AUDIO_HIGHWAY_ALIVE
║ SourceAPI  : AudioSource
║ Synth      : TRACK_1_SOURCE
║ Tracks     : 4 PROCESSORS
║ Gain       : AUDIO_PATH
║ Pan        : AUDIO_PATH
║ Mute       : AUDIO_PATH
║ Solo       : AUDIO_PATH
║ SumBus     : STEREO
║ MasterBus  : AUDIO_PATH
║ Transport  : AUDIO_DRIVEN
║ Build      : True
║ Runtime    : True
║ ExitCode   : $RuntimeExit
║ ExeSHA     : $Hash
║ Fu         : WAITING_AT_CAMP
╚════════════════════════════════════════════════════════════╝
"@

    $Rock | Set-Clipboard

    Write-Host ""
    Write-Host $Rock -ForegroundColor Cyan
    Write-Host ""
    Write-Host "📋 Audio Highway rock hydrated." -ForegroundColor Magenta
}


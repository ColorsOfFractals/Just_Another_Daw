#include "TransportState.h"

#include <algorithm>


void TransportState::play() noexcept
{
    playing.store (true);
}


void TransportState::stop() noexcept
{
    playing.store (false);
    recording.store (false);
}


void TransportState::beginRecording() noexcept
{
    recording.store (true);
    playing.store (true);
}


void TransportState::endRecording() noexcept
{
    recording.store (false);
}


void TransportState::returnToStart() noexcept
{
    positionBeats.store (0.0);
    positionSamples.store (0);
}


void TransportState::setTempo (
    double beatsPerMinute) noexcept
{
    tempoBpm.store (
        std::clamp (
            beatsPerMinute,
            20.0,
            400.0
        )
    );
}


double TransportState::getTempo() const noexcept
{
    return tempoBpm.load();
}


void TransportState::setPositionInBeats (
    double beats) noexcept
{
    positionBeats.store (
        std::max (
            0.0,
            beats
        )
    );
}


double TransportState::getPositionInBeats() const noexcept
{
    return positionBeats.load();
}


void TransportState::setPositionInSamples (
    std::int64_t samples) noexcept
{
    positionSamples.store (
        std::max<std::int64_t> (
            0,
            samples
        )
    );
}


std::int64_t
TransportState::getPositionInSamples() const noexcept
{
    return positionSamples.load();
}


void TransportState::advanceSamples (
    int numSamples,
    double sampleRate) noexcept
{
    if (! playing.load())
        return;

    if (numSamples <= 0)
        return;

    if (sampleRate <= 0.0)
        return;

    const auto samples =
        static_cast<std::int64_t> (
            numSamples
        );

    positionSamples.fetch_add (
        samples
    );

    const auto seconds =
        static_cast<double> (
            numSamples
        )
        / sampleRate;

    const auto beatsPerSecond =
        tempoBpm.load()
        / 60.0;

    auto nextBeat =
        positionBeats.load()
        + seconds
        * beatsPerSecond;

    if (loopEnabled.load())
    {
        const auto start =
            loopStartBeat.load();

        const auto end =
            loopEndBeat.load();

        if (end > start)
        {
            const auto length =
                end - start;

            while (nextBeat >= end)
                nextBeat -= length;
        }
    }

    positionBeats.store (
        nextBeat
    );
}


bool TransportState::isPlaying() const noexcept
{
    return playing.load();
}


bool TransportState::isRecording() const noexcept
{
    return recording.load();
}


void TransportState::setTimeSignature (
    int numerator,
    int denominator) noexcept
{
    timeSignatureNumerator.store (
        std::max (
            1,
            numerator
        )
    );

    timeSignatureDenominator.store (
        std::max (
            1,
            denominator
        )
    );
}


int
TransportState::getTimeSignatureNumerator() const noexcept
{
    return timeSignatureNumerator.load();
}


int
TransportState::getTimeSignatureDenominator() const noexcept
{
    return timeSignatureDenominator.load();
}


void TransportState::setMetronomeEnabled (
    bool enabled) noexcept
{
    metronomeEnabled.store (
        enabled
    );
}


bool TransportState::isMetronomeEnabled() const noexcept
{
    return metronomeEnabled.load();
}


void TransportState::setCountInBars (
    int bars) noexcept
{
    const auto legalBars =
        bars <= 0
            ? 0
            : bars == 1
                ? 1
                : bars <= 2
                    ? 2
                    : 4;

    countInBars.store (
        legalBars
    );
}


int TransportState::getCountInBars() const noexcept
{
    return countInBars.load();
}


void TransportState::setClickDuringPlayback (
    bool enabled) noexcept
{
    clickDuringPlayback.store (
        enabled
    );
}


bool
TransportState::isClickDuringPlaybackEnabled() const noexcept
{
    return clickDuringPlayback.load();
}


void TransportState::setClickDuringRecording (
    bool enabled) noexcept
{
    clickDuringRecording.store (
        enabled
    );
}


bool
TransportState::isClickDuringRecordingEnabled() const noexcept
{
    return clickDuringRecording.load();
}


void TransportState::setAccentFirstBeat (
    bool enabled) noexcept
{
    accentFirstBeat.store (
        enabled
    );
}


bool
TransportState::isAccentFirstBeatEnabled() const noexcept
{
    return accentFirstBeat.load();
}


void TransportState::setMetronomeVolume (
    float volume) noexcept
{
    metronomeVolume.store (
        std::clamp (
            volume,
            0.0f,
            1.0f
        )
    );
}


float TransportState::getMetronomeVolume() const noexcept
{
    return metronomeVolume.load();
}


void TransportState::setMetronomeSound (
    int soundIndex) noexcept
{
    metronomeSound.store (
        std::clamp (
            soundIndex,
            0,
            3
        )
    );
}


int TransportState::getMetronomeSound() const noexcept
{
    return metronomeSound.load();
}


void TransportState::setLoopEnabled (
    bool enabled) noexcept
{
    loopEnabled.store (
        enabled
    );
}


bool TransportState::isLoopEnabled() const noexcept
{
    return loopEnabled.load();
}


void TransportState::setLoopRange (
    double startBeat,
    double endBeat) noexcept
{
    startBeat =
        std::max (
            0.0,
            startBeat
        );

    endBeat =
        std::max (
            startBeat,
            endBeat
        );

    loopStartBeat.store (
        startBeat
    );

    loopEndBeat.store (
        endBeat
    );
}


double TransportState::getLoopStartBeat() const noexcept
{
    return loopStartBeat.load();
}


double TransportState::getLoopEndBeat() const noexcept
{
    return loopEndBeat.load();
}
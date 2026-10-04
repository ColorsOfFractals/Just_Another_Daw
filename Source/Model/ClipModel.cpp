#include "ClipModel.h"

#include <algorithm>
#include <cmath>
#include <utility>

ClipModel::ClipModel (
    ClipId clipId,
    std::string clipName,
    double initialStartBeat,
    double initialLengthBeats)
    : id (clipId),
      name (std::move (clipName))
{
    setStartBeat (initialStartBeat);
    setLengthBeats (initialLengthBeats);
}

ClipModel::ClipId ClipModel::getId() const noexcept
{
    return id;
}

const std::string& ClipModel::getName() const noexcept
{
    return name;
}

void ClipModel::setName (std::string newName)
{
    name = std::move (newName);
}

double ClipModel::getStartBeat() const noexcept
{
    return startBeat.load();
}

void ClipModel::setStartBeat (double beat) noexcept
{
    startBeat.store (
        std::max (0.0, beat)
    );
}

double ClipModel::getLengthBeats() const noexcept
{
    return lengthBeats.load();
}

void ClipModel::setLengthBeats (double beats) noexcept
{
    lengthBeats.store (
        std::max (0.001, beats)
    );
}

bool ClipModel::isMuted() const noexcept
{
    return muted.load();
}

void ClipModel::setMuted (bool shouldBeMuted) noexcept
{
    muted.store (shouldBeMuted);
}


void ClipModel::setMidiEvents (
    std::vector<MidiEvent> events)
{
    const std::scoped_lock lock (
        midiMutex
    );

    midiEvents =
        std::move (events);
}


std::vector<ClipModel::MidiEvent>
ClipModel::getMidiEventsSnapshot() const
{
    const std::scoped_lock lock (
        midiMutex
    );

    return midiEvents;
}


std::size_t ClipModel::getMidiEventCount() const
{
    const std::scoped_lock lock (
        midiMutex
    );

    return midiEvents.size();
}


std::size_t ClipModel::quantizeMidi (
    double gridBeats)
{
    if (gridBeats <= 0.0)
        return 0;

    const std::scoped_lock lock (
        midiMutex
    );

    std::vector<bool> consumedOffEvents (
        midiEvents.size(),
        false
    );

    std::size_t quantizedNotes = 0;

    for (
        std::size_t noteOnIndex = 0;
        noteOnIndex < midiEvents.size();
        ++noteOnIndex
    )
    {
        auto& noteOn =
            midiEvents[noteOnIndex];

        if (! noteOn.noteOn)
            continue;

        const auto snappedBeat =
            std::round (
                noteOn.beat / gridBeats
            ) * gridBeats;

        const auto delta =
            snappedBeat - noteOn.beat;

        noteOn.beat =
            std::max (
                0.0,
                snappedBeat
            );

        for (
            std::size_t noteOffIndex =
                noteOnIndex + 1;
            noteOffIndex < midiEvents.size();
            ++noteOffIndex
        )
        {
            auto& noteOff =
                midiEvents[noteOffIndex];

            if (
                consumedOffEvents[noteOffIndex]
                || noteOff.noteOn
                || noteOff.noteNumber
                    != noteOn.noteNumber
                || noteOff.channel
                    != noteOn.channel
            )
            {
                continue;
            }

            noteOff.beat =
                std::max (
                    noteOn.beat,
                    noteOff.beat + delta
                );

            consumedOffEvents[noteOffIndex] =
                true;

            break;
        }

        ++quantizedNotes;
    }

    std::stable_sort (
        midiEvents.begin(),
        midiEvents.end(),
        [] (
            const MidiEvent& left,
            const MidiEvent& right)
        {
            if (left.beat != right.beat)
                return left.beat < right.beat;

            if (left.noteNumber != right.noteNumber)
                return left.noteNumber < right.noteNumber;

            return ! left.noteOn
                && right.noteOn;
        }
    );

    return quantizedNotes;
}
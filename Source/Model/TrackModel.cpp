#include "TrackModel.h"

#include <algorithm>
#include <utility>

TrackModel::TrackModel (
    TrackId trackId,
    std::string trackName)
    : id (trackId),
      name (std::move (trackName))
{
}

TrackModel::TrackId TrackModel::getId() const noexcept
{
    return id;
}

void TrackModel::setName (
    std::string newName)
{
    name = std::move (newName);
}

const std::string& TrackModel::getName() const noexcept
{
    return name;
}

void TrackModel::setGain (
    float linearGain) noexcept
{
    gain.store (
        std::clamp (
            linearGain,
            0.0f,
            2.0f
        )
    );
}

float TrackModel::getGain() const noexcept
{
    return gain.load();
}

void TrackModel::setPan (
    float panPosition) noexcept
{
    pan.store (
        std::clamp (
            panPosition,
            -1.0f,
            1.0f
        )
    );
}

float TrackModel::getPan() const noexcept
{
    return pan.load();
}

void TrackModel::setMuted (
    bool shouldBeMuted) noexcept
{
    muted.store (shouldBeMuted);
}

bool TrackModel::isMuted() const noexcept
{
    return muted.load();
}

void TrackModel::setSolo (
    bool shouldBeSolo) noexcept
{
    solo.store (shouldBeSolo);
}

bool TrackModel::isSolo() const noexcept
{
    return solo.load();
}

void TrackModel::setRecordArmed (
    bool shouldBeArmed) noexcept
{
    recordArmed.store (shouldBeArmed);
}

bool TrackModel::isRecordArmed() const noexcept
{
    return recordArmed.load();
}

void TrackModel::clearRecordedMidiEvents()
{
    const std::scoped_lock lock (
        recordedMidiMutex
    );

    recordedMidiEvents.clear();
}

void TrackModel::recordMidiEvent (
    bool noteOn,
    int noteNumber,
    float velocity,
    int channel,
    double beat)
{
    const std::scoped_lock lock (
        recordedMidiMutex
    );

    recordedMidiEvents.push_back ({
        noteOn,
        noteNumber,
        velocity,
        channel,
        beat
    });
}

std::size_t TrackModel::getRecordedMidiEventCount() const
{
    const std::scoped_lock lock (
        recordedMidiMutex
    );

    return recordedMidiEvents.size();
}

std::vector<TrackModel::RecordedMidiEvent>
TrackModel::getRecordedMidiEventsSnapshot() const
{
    const std::scoped_lock lock (
        recordedMidiMutex
    );

    return recordedMidiEvents;
}

ClipModel& TrackModel::addClip (
    std::string clipName,
    double startBeat,
    double lengthBeats)
{
    auto clip =
        std::make_unique<ClipModel> (
            nextClipId++,
            std::move (clipName),
            startBeat,
            lengthBeats
        );

    auto& reference = *clip;

    clips.push_back (
        std::move (clip)
    );

    return reference;
}

bool TrackModel::removeClip (
    ClipModel::ClipId clipId)
{
    const auto before =
        clips.size();

    clips.erase (
        std::remove_if (
            clips.begin(),
            clips.end(),
            [clipId] (
                const std::unique_ptr<ClipModel>& clip)
            {
                return clip != nullptr
                    && clip->getId() == clipId;
            }
        ),
        clips.end()
    );

    return clips.size() != before;
}

std::size_t TrackModel::getClipCount() const noexcept
{
    return clips.size();
}

ClipModel* TrackModel::getClip (
    std::size_t index) noexcept
{
    if (index >= clips.size())
        return nullptr;

    return clips[index].get();
}

const ClipModel* TrackModel::getClip (
    std::size_t index) const noexcept
{
    if (index >= clips.size())
        return nullptr;

    return clips[index].get();
}

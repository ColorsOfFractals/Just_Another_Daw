#include "ProjectState.h"

#include <utility>

ProjectState::ProjectState() = default;

const std::string& ProjectState::getTitle() const noexcept
{
    return title;
}

void ProjectState::setTitle (
    std::string newTitle)
{
    title = std::move (newTitle);
    markDirty();
}

bool ProjectState::isDirty() const noexcept
{
    return dirty;
}

void ProjectState::markDirty() noexcept
{
    dirty = true;
    ++revision;
}

void ProjectState::markSaved() noexcept
{
    dirty = false;
}

std::uint64_t ProjectState::getRevision() const noexcept
{
    return revision;
}

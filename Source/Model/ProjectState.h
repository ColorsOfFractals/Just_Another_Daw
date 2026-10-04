#pragma once

#include <cstdint>
#include <string>

class ProjectState
{
public:
    ProjectState();

    const std::string& getTitle() const noexcept;
    void setTitle (std::string newTitle);

    bool isDirty() const noexcept;
    void markDirty() noexcept;
    void markSaved() noexcept;

    std::uint64_t getRevision() const noexcept;

private:
    std::string title { "Untitled JAD Session" };

    bool dirty = false;
    std::uint64_t revision = 1;
};

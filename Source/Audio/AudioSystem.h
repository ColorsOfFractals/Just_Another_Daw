#pragma once

class AudioSystem
{
public:
    AudioSystem();
    ~AudioSystem();

    bool isAlive() const noexcept;

private:
    bool alive = false;
};

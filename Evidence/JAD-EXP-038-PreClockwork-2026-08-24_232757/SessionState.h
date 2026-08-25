#pragma once

class SessionState
{
public:
    SessionState();

    bool isAlive() const noexcept;

private:
    bool alive = false;
};

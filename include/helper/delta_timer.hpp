#pragma once
#include <chrono>

class DeltaTimer
{
public:
    DeltaTimer();

    void update();

    operator float();

    float get();

private:
    std::chrono::time_point<std::chrono::system_clock> last_update{};
    float delta_time{0};
};
#pragma once
#include <chrono>

class DeltaTimer
{
public:
    DeltaTimer()
        : last_update{std::chrono::system_clock::now()}
    {
    }

    void update()
    {
        const auto current_time = std::chrono::system_clock::now();
        delta_time = std::chrono::duration<float>(current_time - last_update).count();
        last_update = current_time;
    }

    operator float()
    {
        return delta_time;
    }

    float get()
    {
        return delta_time;
    }

private:
    std::chrono::time_point<std::chrono::system_clock> last_update{};
    float delta_time{0};
};
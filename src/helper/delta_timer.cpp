#include <helper/delta_timer.hpp>

DeltaTimer::DeltaTimer()
        : last_update{std::chrono::system_clock::now()}
    {
    }

    void DeltaTimer::update()
    {
        const auto current_time = std::chrono::system_clock::now();
        delta_time = std::chrono::duration<float>(current_time - last_update).count();
        last_update = current_time;
    }

    DeltaTimer::operator float()
    {
        return delta_time;
    }

    float DeltaTimer::get()
    {
        return delta_time;
    }

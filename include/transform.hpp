#pragma once

struct Transform
{
    Transform() = default;
    Transform(const float& x,const float& y,const float& rotation)
    :x{x},y{y},rotation{rotation}
    {}


    static Transform interpolate(const Transform& from, const Transform& to, const float& t)
    {
        const auto diff_x = to.x - from.x;
        const auto diff_y = to.y - from.y;
        const auto diff_rotation = to.rotation - from.rotation;

        return {from.x + diff_x * t,from.y + diff_y * t,from.rotation + diff_rotation * t};
    }

    static Transform predictNextPosition(const Transform& last_message, const Transform& current_message)
    {
        return interpolate(last_message, current_message, 2);
    }

    Transform moveTowardsNextPrediction(const Transform& last_message, const Transform& current_message, const float& time_delta_between_message, const float& time_since_last_message)
    {
        return interpolate(*this, predictNextPosition(last_message, current_message), time_since_last_message / time_delta_between_message);
    }


    float x{},y{};

    float rotation{};
};
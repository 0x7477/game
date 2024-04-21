#include <game/movement_animation.hpp>
#include <game/map.hpp>

TileIndex MovementAnimation::getEndPosition()
{
    return end;
}

std::tuple<float, float> MovementAnimation::getCurrentPosition(const Map &map)
{
    delta_time.update();
    time_passed += delta_time;

    const auto [start_x_index, start_y_index] = start;
    const auto [end_x_index, end_y_index] = end;
    const auto [start_x, start_y] = map.getScreenPosition(start);
    const auto [end_x, end_y] = map.getScreenPosition(end);

    float dx = end_x - start_x;
    float dy = end_y - start_y;

    const unsigned tile_distance  = abs(end_x_index - start_x_index) + abs(end_y_index - start_y_index);

    float percent{std::clamp(time_passed / (tile_distance / tiles_per_second), 0.f, 1.f)};

    if (percent == 1 && on_animation_finished)
    {
        (*on_animation_finished)();
        on_animation_finished = {};
    }

    return {start_x + dx * percent, start_y + dy * percent};
}
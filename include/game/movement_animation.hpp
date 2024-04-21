#pragma once

#include <game/tile_index.hpp>
#include <helper/delta_timer.hpp>
#include <functional>
#include <optional>
class Map;

class MovementAnimation
{
public:
    MovementAnimation(const TileIndex &start, const TileIndex &end, const std::function<void()>& on_animation_finished)
        : start{start}, end{end},on_animation_finished{on_animation_finished}
    {
    }

    TileIndex getEndPosition();

    std::tuple<float, float> getCurrentPosition(const Map& map);



private:
    TileIndex start, end;
    float tiles_per_second{5};
    float time_passed{0};
    DeltaTimer delta_time;
    std::optional<std::function<void()>> on_animation_finished;
};
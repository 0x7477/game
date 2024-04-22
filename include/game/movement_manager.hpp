#pragma once

#include <game/tile_index.hpp>
#include <game/movement.hpp>
#include <helper/delta_timer.hpp>
#include <functional>
#include <optional>
#include <SFML/Graphics.hpp>

class Map;
class Unit;

class MovementManager
{
public:
    MovementManager(const Unit& unit);

    void init(const TileIndex &start);
    void startAnimation(const std::function<void()>& on_animation_finished);

    TileIndex getEndPosition();

    bool isMoving() const;

    void stop();

    std::tuple<float, float> getCurrentPosition(const Map& map, const TileIndex& pos);

    void displayPath(sf::RenderWindow& window, Map& map);

    void updatePath(Map& map, const TileIndex& tile);


    void recalculatePath(Map &map, const TileIndex &start, const TileIndex &end);

private:

    void updatePathCosts(Map& map);
    void initPathSprites();

    static std::optional<std::map<std::tuple<int,int>, sf::Sprite>> path_sprites;

    const Unit& unit;
    bool is_moving{false};
    unsigned current_movement_costs{0};
    std::vector<TileIndex> path{};
    float seconds_per_tile{0.2};
    float time_passed{0};
    DeltaTimer delta_time;
    std::optional<std::function<void()>> on_animation_finished;
};
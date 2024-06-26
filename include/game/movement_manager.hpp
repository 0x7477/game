#pragma once

#include <game/tile_index.hpp>
#include <game/movement.hpp>
#include <helper/delta_timer.hpp>
#include <functional>
#include <optional>
#include <SFML/Graphics.hpp>
#include <ui/gif.hpp>
class Map;
class Unit;

class MovementManager
{
public:
    MovementManager(const Unit& unit);

    void init(const TileIndex &start);
    void startAnimation(const std::function<void()>& on_animation_finished);

    TileIndex getEndPosition() const;
    std::vector<TileIndex> getPath() const;

    bool isMoving() const;

    void draw(sf::RenderWindow &window, const Map &map, const TileIndex &index);
    void stop();



    std::tuple<float, float> getCurrentPosition(const Map& map, const TileIndex& pos);

    void displayPath(sf::RenderWindow& window, Map& map);

    void updatePath(Map& map, const TileIndex& tile);
    void setPath(const std::vector<TileIndex>& path);


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
    UI::GIF idle, up, down, left, right;
    UI::GIF* current_animation{&idle};
    std::optional<std::function<void()>> on_animation_finished;
};
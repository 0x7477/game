#include <game/movement_manager.hpp>
#include <game/movement.hpp>
#include <game/map.hpp>
#include <game/tile_selector.hpp>
#include <cmath>
MovementManager::MovementManager(const Unit &unit)
    : unit{unit}, 
    idle{"units/" + unit.id + "/" + std::to_string(unit.getTeam()) + "/idle/", "resources/images/"},
    up{"units/" + unit.id + "/" + std::to_string(unit.getTeam()) + "/up/", "resources/images/"},
    down{"units/" + unit.id + "/" + std::to_string(unit.getTeam()) + "/down/", "resources/images/"},
    left{"units/" + unit.id + "/" + std::to_string(unit.getTeam()) + "/left/", "resources/images/"},
    right{"units/" + unit.id + "/" + std::to_string(unit.getTeam()) + "/right/", "resources/images/"}
{
}
void MovementManager::draw(sf::RenderWindow &window, const Map &map, const TileIndex &index)
{
    const auto offset = map.scale * 3.0;

    const auto [x, y] = getCurrentPosition(map, index);

    current_animation->setColor(unit.status.finished ? sf::Color(100, 100, 100) : sf::Color::White);
    current_animation->display(window, x, y - offset, map.scale);


}
void MovementManager::init(const TileIndex &start)
{
    path = {start};
    current_movement_costs = 0;
}
void MovementManager::startAnimation(const std::function<void()> &on_animation_finished_)
{
    is_moving = true;
    on_animation_finished = on_animation_finished_;
    delta_time.update();
    time_passed = 0;
}

void MovementManager::stop()
{
    is_moving = false;
}

TileIndex MovementManager::getEndPosition() const
{
    return path[path.size() - 1];
}

void MovementManager::setPath(const std::vector<TileIndex> &path_)
{
    path = path_;
}

std::vector<TileIndex> MovementManager::getPath() const
{
    return path;
}

void MovementManager::updatePath(Map &map, const TileIndex &index)
{
    // std::cout << __FILE__ << ":" << __LINE__ << "\n";
    if (index == getEndPosition())
        return;

    const auto movement_costs = map[index].getMovementCost(unit.getMovementType());

    const auto cant_go_through_tile = (map[index].unit && map[index].unit->getTeam() != map.team) ||  movement_costs == 0;
    if (cant_go_through_tile)
        return;

    const auto path_index = std::find(path.begin(), path.end(), index);
    if (path_index != std::end(path)) // we are moving to a part we already explored
    {
        path.resize(std::distance(path.begin(), path_index) + 1);
        updatePathCosts(map);

        return;
    }

    // test if we can expand
    const auto are_tiles_adjacent = TileIndex::areTileIndexesAdjacent(getEndPosition(), index);
    if (!are_tiles_adjacent)
    {
        recalculatePath(map, path[0], index);
        updatePathCosts(map);
        return;
    }

    const auto costs_needed_to_reach_new_tile = map[index].getMovementCost(unit.getMovementType());
    if (unit.getMovementSpeed() < costs_needed_to_reach_new_tile + current_movement_costs)
    {
        // check if we can optimize our path
        recalculatePath(map, path[0], index);
        updatePathCosts(map);
        return;
    }

    current_movement_costs += costs_needed_to_reach_new_tile;
    path.push_back(index);
}

void MovementManager::updatePathCosts(Map &map)
{
    current_movement_costs = 0;

    for (unsigned i = 1; i < path.size(); i++)
        current_movement_costs += map[path[i]].getMovementCost(unit.getMovementType());
}

bool MovementManager::isMoving() const
{
    return is_moving;
}

void MovementManager::recalculatePath(Map &map, const TileIndex &start, const TileIndex &end)
{
    const auto result = MovementSelector::getPath(map, start, end, unit);
    if (!result)
        return;
    path = *result;
}

std::tuple<float, float> MovementManager::getCurrentPosition(const Map &map, const TileIndex &pos)
{
    if (!isMoving())
    {
        current_animation = &idle;
        return map.getScreenPosition(pos);
    }

    delta_time.update();

    time_passed += delta_time;

    const auto current_tile = std::min((std::size_t)(time_passed / seconds_per_tile), path.size() - 2);
    const auto start = path[current_tile];
    const auto end = path[current_tile + 1];

    const auto [start_x, start_y] = map.getScreenPosition(start);
    const auto [end_x, end_y] = map.getScreenPosition(end);

    float dx = end_x - start_x;
    float dy = end_y - start_y;

    // std::cout << ((int)start.x - (int)end.x) << " " << ((int)start.y - (int)end.y) << "\n";
    if(path.size() > 1)
    {
    if(((int)start.x - (int)end.x) > 0)
        current_animation = &left;
    else if(((int)start.x - (int)end.x) < 0)
        current_animation = &right;
    else if(((int)start.y - (int)end.y) > 0)
        current_animation = &up;
    else if(((int)start.y - (int)end.y) < 0)
        current_animation = &down;
    }
    else
    {
        current_animation = &down;
    }


    float percent{std::fmod(time_passed, seconds_per_tile) / seconds_per_tile};

    if (time_passed >= (path.size() - 1) * seconds_per_tile)
    {

        if (on_animation_finished)
        {
            (*on_animation_finished)();
            on_animation_finished = {};
        }
        return map.getScreenPosition(path[path.size() - 1]);
    }

    return {start_x + dx * percent, start_y + dy * percent};
}

void MovementManager::displayPath(sf::RenderWindow &window, Map &map)
{
    if (!path_sprites)
        initPathSprites();
    if (path.size() <= 1)
        return;

    for (unsigned i{0}; i < path.size() - 1; i++)
    {
        const auto first_tile = path[i];
        const auto next_tile = path[i + 1];

        int dx = next_tile.x - first_tile.x;
        int dy = next_tile.y - first_tile.y;

        const auto is_last_tile = path.size() == i + 2;
        if (!is_last_tile)
        {
            const auto next_next_tile = path[i + 2];
            dx += 2 * (next_next_tile.x - next_tile.x);
            dy += 2 * (next_next_tile.y - next_tile.y);
        }

        const auto [x, y] = map.getScreenPosition(next_tile);
        auto &sprite = (*path_sprites)[{dx, dy}];
        sprite.setPosition(x, y);
        sprite.setScale(map.scale, map.scale);
        window.draw(sprite);
    }
}

std::optional<std::map<std::tuple<int, int>, sf::Sprite>> MovementManager::path_sprites;

void MovementManager::initPathSprites()
{
    path_sprites = std::map<std::tuple<int, int>, sf::Sprite>{
        {{-1, 0}, sf::Sprite{image_resources.get("misc/path/left_end.png")}},
        {{0, -1}, sf::Sprite{image_resources.get("misc/path/up_end.png")}},
        {{1, 0}, sf::Sprite{image_resources.get("misc/path/right_end.png")}},
        {{0, 1}, sf::Sprite{image_resources.get("misc/path/down_end.png")}},
        {{3, 0}, sf::Sprite{image_resources.get("misc/path/horizontal.png")}},
        {{-3, 0}, sf::Sprite{image_resources.get("misc/path/horizontal.png")}},
        {{0, 3}, sf::Sprite{image_resources.get("misc/path/vertical.png")}},
        {{0, -3}, sf::Sprite{image_resources.get("misc/path/vertical.png")}},
        {{1, -2}, sf::Sprite{image_resources.get("misc/path/left_up.png")}},
        {{1, 2}, sf::Sprite{image_resources.get("misc/path/down_left.png")}},
        {{-1, 2}, sf::Sprite{image_resources.get("misc/path/right_down.png")}},
        {{-2, -1}, sf::Sprite{image_resources.get("misc/path/down_left.png")}},
        {{2, 1}, sf::Sprite{image_resources.get("misc/path/up_right.png")}},
        {{-2, 1}, sf::Sprite{image_resources.get("misc/path/left_up.png")}},
        {{-1, -2}, sf::Sprite{image_resources.get("misc/path/up_right.png")}},
        {{2, -1}, sf::Sprite{image_resources.get("misc/path/right_down.png")}},

    };
}

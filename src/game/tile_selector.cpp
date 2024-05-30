#include <game/tile_selector.hpp>
#include <game/unit.hpp>
#include <game/map.hpp>
#include <game/attack_simulator.hpp>
#include <set>

void MovementSelector::fill(Map &map, const TileIndex &index, const int &remaining_movement_speed, const Unit &unit, std::vector<TileIndex> &tile_indices, std::map<TileIndex, std::tuple<int, TileIndex>> &discovered_movement_costs, const TileIndex &last_index)
{
    if (remaining_movement_speed < 0)
        return;

    if (discovered_movement_costs.contains(index) && remaining_movement_speed <= std::get<int>(discovered_movement_costs[index]))
        return;

    auto &tile = map[index];

    bool is_movement_possible = true;
    if (tile.unit)
    {
        if (tile.unit->getTeam() != unit.getTeam())
            return;
        is_movement_possible = tile.unit->allowUnitInteraction(unit);
    }

    discovered_movement_costs[index] = {remaining_movement_speed, last_index};

    if (is_movement_possible)
        tile_indices.push_back(index);

    for (const auto &neighbour : map.getNeighbors(index))
    {
        const auto movement_cost = map[neighbour].getMovementCost(unit.getMovementType());

        if (movement_cost == 0)
            continue;
        fill(map, neighbour, remaining_movement_speed - movement_cost, unit, tile_indices, discovered_movement_costs, index);
    }
}

std::vector<TileIndex> MovementSelector::getTiles(Map &map, const TileIndex &tile_index, const Unit &unit)
{
    std::vector<TileIndex> tiles{tile_index};
    std::map<TileIndex, std::tuple<int, TileIndex>> discovered_movement_costs{};
    fill(map, tile_index, unit.getMovementSpeed(), unit, tiles, discovered_movement_costs, tile_index);
    return tiles;
}

std::optional<std::vector<TileIndex>> MovementSelector::getPath(Map &map, const TileIndex &start, const TileIndex &end, const Unit &unit)
{
    std::vector<TileIndex> path{start};
    std::map<TileIndex, std::tuple<int, TileIndex>> discovered_movement_costs{};
    fill(map, start, unit.getMovementSpeed(), unit, path, discovered_movement_costs, start);

    if (!discovered_movement_costs.contains(end))
        return {};

    path.clear();

    TileIndex current_tile = end;
    while (current_tile != start)
    {
        path.push_back(current_tile);
        assert(discovered_movement_costs.contains(current_tile));
        current_tile = std::get<TileIndex>(discovered_movement_costs[current_tile]);
    }
    path.push_back(start);

    std::reverse(path.begin(), path.end());
    return path;
}

std::vector<TileIndex> AttackSelector::getTiles(Map &map, const TileIndex &tile_index, const Unit &unit, const unsigned &minimum_range, const unsigned &maximum_range)
{
    std::vector<TileIndex> tiles{};

    for (unsigned y = std::min(0u, tile_index.y - maximum_range); y < std::max(map.height-1, tile_index.y + maximum_range); y++)
    {
        for (unsigned x = std::min(0u, tile_index.x - maximum_range); x < std::max(map.width-1, tile_index.x + maximum_range); x++)
        {
            const unsigned abs_distance = abs((int)x - (int)tile_index.x) + abs((int)y - (int)tile_index.y);
            if (abs_distance < minimum_range || abs_distance > maximum_range)
                continue;

            if (map[x, y].isAttackable(map, {x, y}, unit))
                tiles.push_back({x, y});

            if (!map[x, y].unit)
                continue;

            if (map[x, y].unit->getTeam() == unit.getTeam())
                continue;

            if (!AttackSimulator::canAttack(unit, *map[x, y].unit))
                continue;

            tiles.push_back({x, y});
        }
    }

    return tiles;
}

std::vector<TileIndex> UnloadSelector::getTiles(Map &map, const TileIndex &tile_index, const Unit &unit)
{
    std::vector<TileIndex> tiles{};

    for (const auto &neighbour : map.getNeighbors(tile_index))
    {
        if (map[neighbour].getMovementCost(unit.getMovementType()) == 0)
            continue;

        if (map[neighbour].unit)
            continue;

        tiles.push_back(neighbour);
    }

    return tiles;
}

std::vector<TileIndex> AttackableSelector::getTiles(Map &map, const TileIndex &tile_index, const Unit &unit)
{
    std::set<TileIndex> indices;
    const auto moveable_tiles = MovementSelector::getTiles(map, tile_index, unit);
    for (const auto &tile : moveable_tiles)
    {
        const auto maximum_range = unit.stats.attack_range_max;
        const auto minimum_range = unit.stats.attack_range_min;
        for (unsigned y = std::min(0u, tile.y - maximum_range); y < std::max(map.height-1, tile.y + maximum_range); y++)
        {
            for (unsigned x = std::min(0u, tile.x - maximum_range); x < std::max(map.width-1, tile.x + maximum_range); x++)
            {
                const unsigned abs_distance = abs((int)x - (int)tile.x) + abs((int)y - (int)tile.y);
                if (abs_distance < minimum_range || abs_distance > maximum_range)
                    continue;

                indices.emplace(TileIndex{x, y});
            }
        }
    }

    return {indices.begin(), indices.end()};
}

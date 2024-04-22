#include <game/tile_selector.hpp>
#include <game/unit.hpp>
#include <game/map.hpp>
#include <game/attack_simulator.hpp>

void MovementSelector::fill(Map &map, const TileIndex &index, const int &remaining_movement_speed, const Unit &unit, std::vector<TileIndex> &tile_indices, std::map<TileIndex, std::tuple<int, TileIndex>> &discovered_movement_costs, const TileIndex &last_index)
{
    if (remaining_movement_speed < 0)
        return;

    if (discovered_movement_costs.contains(index) && remaining_movement_speed <= std::get<int>(discovered_movement_costs[index]))
        return;

    std::get<int>(discovered_movement_costs[index]) = remaining_movement_speed;
    std::get<TileIndex>(discovered_movement_costs[index]) = last_index;

    auto &tile = map[index];

    bool is_movement_possible = true;
    if (tile.unit)
    {
        if (tile.unit->getTeam() != unit.getTeam())
            return;
        is_movement_possible = tile.unit->allowUnitInteraction(unit);
    }

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
    fill(map, tile_index, unit.getMovementSpeed(), unit, tiles, discovered_movement_costs,tile_index);
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

    // int i = 10;
    while (current_tile != start)
    {
        // std::cout << current_tile << "\n";
        // if(i-- == 0)
        //     break;
        path.push_back(current_tile);
        current_tile = std::get<TileIndex>(discovered_movement_costs[current_tile]);

    }
    path.push_back(start);
    
    std::reverse(path.begin(), path.end());
    // std::getchar();
    return path;
}

std::vector<TileIndex> AttackSelector::getTiles(Map &map, const TileIndex &tile_index, const Unit &unit, const unsigned &minimum_range, const unsigned &maximum_range)
{
    std::vector<TileIndex> tiles{};

    const auto [tile_x, tile_y] = tile_index;
    for (unsigned y = std::min(0u, tile_y - maximum_range); y < std::max(map.height, tile_y + maximum_range); y++)
    {
        for (unsigned x = std::min(0u, tile_x - maximum_range); x < std::max(map.width, tile_x + maximum_range); x++)
        {
            const unsigned abs_distance = abs((int)x - (int)tile_x) + abs((int)y - (int)tile_y);
            if (abs_distance < minimum_range || abs_distance > maximum_range)
                continue;

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
        if (map.getTile(neighbour).getMovementCost(unit.getMovementType()) == 0)
            continue;

        if (map.getTile(neighbour).unit)
            continue;

        tiles.push_back(neighbour);
    }

    return tiles;
}
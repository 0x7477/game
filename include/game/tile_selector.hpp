#pragma once
#include <vector>
#include <tuple>
#include <game/tile_index.hpp>
#include <map>
class Map;
class Tile;
class Unit;

class MovementSelector
{
    static void fill(Map &map, const TileIndex &index, const int &remaining_movement_speed, const Unit &unit, std::vector<TileIndex> &tile_indices, std::map<TileIndex, int> &discovered_movement_costs);

public:
    static std::vector<TileIndex> getTiles(Map &map, const TileIndex &tile_index, const Unit &unit);
};

class AttackSelector
{
public:
    static std::vector<TileIndex> getTiles(Map &map, const TileIndex &tile_index, const Unit &unit, const unsigned &minimum_range, const unsigned &maximum_range);
};

class UnloadSelector
{
public:
    static std::vector<TileIndex> getTiles(Map &map, const TileIndex &tile_index, const Unit &unit);
};
#pragma once
#include <vector>
#include <tuple>
#include <game/tile_index.hpp>
#include <map>
#include <optional>
class Map;
class Tile;
class Unit;

class MovementSelector
{
    static void fill(Map &map, const TileIndex &index, const int &remaining_movement_speed, const Unit &unit, std::vector<TileIndex> &tile_indices, std::map<TileIndex, std::tuple<int, TileIndex>> &discovered_movement_costs,const TileIndex &last_index, const bool& visual = false);

public:
    static std::vector<TileIndex> getTiles(Map &map, const TileIndex &tile_index, const Unit &unit, const bool& visual = false);
    static std::optional<std::vector<TileIndex>> getPath(Map &map, const TileIndex &start, const TileIndex &end, const Unit &unit);
};



class AttackSelector
{
public:
    static std::vector<TileIndex> getTiles(Map &map, const TileIndex &tile_index, const Unit &unit, const unsigned &minimum_range, const unsigned &maximum_range);
};

class AttackableSelector
{
public:
    static std::vector<TileIndex> getTiles(Map &map, const TileIndex &tile_index, const Unit &unit);
};

class UnloadSelector
{
public:
    static std::vector<TileIndex> getTiles(Map &map, const TileIndex &tile_index, const Unit &unit);
};
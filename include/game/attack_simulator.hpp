#pragma once
#include <game/tile_index.hpp>
#include <map>
#include <tuple>
#include <optional>
#include <random>

class Unit;
class Map;

class AttackSimulator
{
    struct AttackResult
    {
        bool attacker_died{false}, defender_died{false};
    };

    struct DamageValues
    {
        DamageValues() = default;
        DamageValues(const std::optional<int>& primary, const std::optional<int>& secondary);
        std::optional<int> primary, secondary;
    };

    static std::tuple<unsigned, bool> calculateDamage(Map& map, const TileIndex& attacker, const TileIndex& defender);

public:
    static bool canAttack(const Unit& attacker, const Unit& defender);
    static AttackResult attack(Map& map, const TileIndex& attacker, const TileIndex& defender);
private:
    static std::minstd_rand rand;
    static std::map<std::tuple<std::string,std::string>, DamageValues> damage_table;
};
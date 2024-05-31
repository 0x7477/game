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

public:
    struct AttackPossibilities
    {
        unsigned attack_min{0}, attack_max{0}, defender_min{0}, defender_max{0};
        bool can_counterattack{false};
    };

    enum DamageCalculationType
    {
        RANDOM, MIN, MAX
    };

    struct DamageValues
    {
        DamageValues() = default;
        DamageValues(const std::optional<int>& primary, const std::optional<int>& secondary);
        std::optional<int> primary, secondary;
    };

    static bool canDefenderCounterattack(Map &map, const TileIndex &attacker_index, const TileIndex &defender_index, const unsigned& damage_done);
    static bool canAttack(const Unit& attacker, const Unit& defender);
    static AttackResult attack(Map& map, const TileIndex& attacker, const TileIndex& defender);
    static AttackPossibilities calculatePossibleDamageValues(Map& map, const TileIndex& attacker, const TileIndex& defender);
private:
    static std::tuple<unsigned, bool> calculateDamage(Map& map, const TileIndex& attacker, const TileIndex& defender,const DamageCalculationType& calculation_type = DamageCalculationType::RANDOM,const std::optional<unsigned>& attacker_health_override = {});
    static std::minstd_rand rand;
    static std::map<std::tuple<std::string,std::string>, DamageValues> damage_table;
};
#include <game/attack_simulator.hpp>
#include <game/map.hpp>
#include <game/unit.hpp>
#include <cmath>

bool AttackSimulator::canAttack(const Unit &attacker, const Unit &defender)
{
    if (!damage_table.contains({attacker.id, defender.id}))
        return false;

    const auto &damage_value = damage_table[{attacker.id, defender.id}];
    if (attacker.ammo == 0 && !damage_value.secondary)
        return false;

    return true;
}

std::tuple<unsigned, bool> AttackSimulator::calculateDamage(Map &map, const TileIndex &attacker_index, const TileIndex &defender_index, const DamageCalculationType &calculation_type, const std::optional<unsigned> &attacker_health_override)
{
    const auto &attacker = map[attacker_index].unit;
    const auto &defender = map[defender_index].unit;

    const std::tuple attacker_damage_table_key{attacker->id, defender->id};

    assert(damage_table.contains(attacker_damage_table_key));
    const auto &damage_values = damage_table[attacker_damage_table_key];
    const auto can_use_primary = attacker->ammo > 0;

    const bool does_use_primary = can_use_primary && damage_values.primary;

    assert(damage_values.primary || damage_values.secondary);

    const auto base_damage = does_use_primary ? *damage_values.primary : *damage_values.secondary;
    const auto attack_value = 100;

    std::uniform_real_distribution<> luck_distribution(0, 9);
    int luck;
    if (calculation_type == MIN)
        luck = luck_distribution.min();
    else if (calculation_type == MAX)
        luck = luck_distribution.max();
    else
        luck = luck_distribution(rand);

    std::uniform_real_distribution<> bad_luck_distribution(0, 0);
    int bad_luck;
    if (calculation_type == MIN)
        bad_luck = bad_luck_distribution.max();
    else if (calculation_type == MAX)
        bad_luck = bad_luck_distribution.min();
    else
        bad_luck = bad_luck_distribution(rand);

    const auto attacker_unit_count = attacker_health_override ? *attacker_health_override : attacker->getUnitCount();

    const auto defense_value = 100;
    const auto terrain_defense = defender->receivesTerrainBonus() ? map[defender_index].getDefense() : 0;

    const auto defender_unit_count = defender->getUnitCount();

    const float potential_damage_from_attacker = (float)base_damage * (float)attack_value / 100.0f + luck - bad_luck;
    const float real_damage_from_attacker = potential_damage_from_attacker * attacker_unit_count / 10.f;
    const float defense = (200.f - (defense_value + terrain_defense * defender_unit_count)) / 100.f;

    const float afflicted_damage = real_damage_from_attacker * defense;

    return {afflicted_damage, does_use_primary};
}

bool AttackSimulator::canDefenderCounterattack(Map &map, const TileIndex &attacker_index, const TileIndex &defender_index, const unsigned &damage_done)
{
    const auto &attacker = map[attacker_index].unit;
    const auto &defender = map[defender_index].unit;

    const auto defender_died = defender->health < (int)damage_done;
    std::cout << defender->health << " " << damage_done << "\n";

    if (defender_died)
        return false;

    const auto can_defender_attack_attacker = damage_table.contains({defender->id, attacker->id});
    if (!can_defender_attack_attacker)
        return false;

    const auto was_direct_attack = TileIndex::areTileIndexesAdjacent(attacker_index, defender_index);

    if (!was_direct_attack)
        return false;

    if (defender->isRangedUnit())
        return false;

    return true;
}

AttackSimulator::AttackResult AttackSimulator::attack(Map &map, const TileIndex &attacker_index, const TileIndex &defender_index)
{
    const auto &attacker = map[attacker_index].unit;
    const auto &defender = map[defender_index].unit;

    const auto [calculated_damage, used_ammo] = calculateDamage(map, attacker_index, defender_index);

    const auto does_defender_counterattack = canDefenderCounterattack(map, attacker_index, defender_index, calculated_damage);
    if (used_ammo)
        attacker->ammo--;

    defender->health -= calculated_damage;
    const auto defender_died = defender->health <= 0;

    if (does_defender_counterattack)
    {
        const auto [counterattack_calculated_damage, counterattack_used_ammo] = calculateDamage(map, defender_index, attacker_index);
        if (counterattack_used_ammo)
            defender->ammo--;

        attacker->health -= counterattack_calculated_damage;
    }

    const auto attacker_died = attacker->health <= 0;

    return {.attacker_died = attacker_died, .defender_died = defender_died};
}

AttackSimulator::DamageValues::DamageValues(const std::optional<int> &primary, const std::optional<int> &secondary)
    : primary{primary}, secondary{secondary} {}

std::minstd_rand AttackSimulator::rand{0};

AttackSimulator::AttackPossibilities AttackSimulator::calculatePossibleDamageValues(Map &map, const TileIndex &attacker, const TileIndex &defender)
{
    AttackPossibilities possibilities;

    possibilities.attack_min = std::get<unsigned>(calculateDamage(map, attacker, defender, DamageCalculationType::MIN));
    possibilities.attack_max = std::get<unsigned>(calculateDamage(map, attacker, defender, DamageCalculationType::MAX));
    
    possibilities.can_counterattack = canDefenderCounterattack(map, attacker, defender, possibilities.attack_max);

    if (possibilities.can_counterattack)
    {
        const auto defender_health = map[defender].unit->getHealth();
        const int defender_health_max = std::max(0,((int)defender_health - (int)possibilities.attack_max) / 10 );
        const int defender_health_min = std::max(0,((int)defender_health - (int)possibilities.attack_min) / 10 );

        possibilities.defender_max = std::get<unsigned>(calculateDamage(map, defender, attacker, DamageCalculationType::MAX, defender_health_max));
        possibilities.defender_min = std::get<unsigned>(calculateDamage(map, defender, attacker, DamageCalculationType::MIN, defender_health_min));
    }
    return possibilities;
}

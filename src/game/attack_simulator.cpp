#include <game/attack_simulator.hpp>
#include <game/map.hpp>
#include <game/unit.hpp>
#include <cmath>

bool AttackSimulator::canAttack(const Unit& attacker, const Unit& defender)
{
    if(!damage_table.contains({attacker.id,defender.id}))
        return false;

    const auto& damage_value = damage_table[{attacker.id,defender.id}];
    if(attacker.ammo == 0 && !damage_value.secondary)
        return false;

    return true;
}

std::tuple<unsigned, bool> AttackSimulator::calculateDamage(Map &map, const TileIndex &attacker_index, const TileIndex &defender_index)
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
    const auto luck = luck_distribution(rand);

    std::uniform_real_distribution<> bad_luck_distribution(0, 0);
    const auto bad_luck = bad_luck_distribution(rand);

    const auto attacker_unit_count = attacker->getUnitCount();

    const auto defense_value = 100;
    const auto terrain_defense = map[defender_index].getDefense();

    const auto defender_unit_count = defender->getUnitCount();

    const float potential_damage_from_attacker = (float)base_damage * (float)attack_value / 100.0f + luck - bad_luck;
    const float real_damage_from_attacker = potential_damage_from_attacker * attacker_unit_count / 10.f;
    const float defense = (200.f - (defense_value + terrain_defense * defender_unit_count)) / 100.f;

    const float afflicted_damage = real_damage_from_attacker * defense;

    return {afflicted_damage, does_use_primary};
}

AttackSimulator::AttackResult AttackSimulator::attack(Map &map, const TileIndex &attacker_index, const TileIndex &defender_index)
{
    const auto &attacker = map[attacker_index].unit;
    const auto &defender = map[defender_index].unit;

    const auto [calculated_damage, used_ammo] = calculateDamage(map, attacker_index, defender_index);
    if (used_ammo)
        attacker->ammo--;

    defender->health -= calculated_damage;

    const auto defender_died = defender->health <= 0;
    const auto can_defender_attack_attacker = damage_table.contains({defender->id, attacker->id});

    const auto tile_index_dx = std::abs((int)std::get<0>(attacker_index) - (int)std::get<0>(defender_index));
    const auto tile_index_dy = std::abs((int)std::get<1>(attacker_index) - (int)std::get<1>(defender_index));
    const auto was_direct_attack = (tile_index_dx + tile_index_dy) == 1;

    const auto does_defender_counterattack = !defender_died && can_defender_attack_attacker && was_direct_attack;

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
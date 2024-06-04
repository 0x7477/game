#include <game/tile.hpp>
#include <game/map.hpp>
#include <map>
#include <game/attack_simulator.hpp>
#include <game/unit_names.hpp>
namespace Tiles
{
    class PipeSeam;
}

std::map<std::string, AttackSimulator::DamageValues> damage_table{
    {{ANTIAIR}, {{}, 5}},
    {{ARTILLERY}, {40, {}}},
    {{BATTLECOPTER}, {20, 1}},
    {{BATTLESHIP}, {50, {}}},
    {{BOMBER}, {90, {}}},
    {{INFANTRY}, {{}, 1}},
    {{MEDIUMTANK}, {45, 1}},
    {{MECH}, {15, 1}},
    {{MEGATANK}, {115, 1}},
    {{NEOTANK}, {55, 1}},
    {{PIPERUNNER}, {50, {}}},
    {{RECON}, {{}, 1}},
    {{ROCKET}, {50, {}}},
    {{STEALTH}, {60, {}}},
    {{TANK}, {15, 1}},
};

struct PipeSeamAttackInteraction : public TileAttackInteraction
{
    bool isAttackable(Map &map, const TileIndex &index, const Unit &unit) const override;
    void attack(Map &map, const TileIndex &unit, const TileIndex &tile) const override;
};

PipeSeamAttackInteraction pipe_seam_attack_interaction;

namespace Tiles
{
    class PipeSeam : public Tile
    {
    public:
        PipeSeam()
            : Tile({.defense = 0, .movement_costs = MovementCosts{{0, 0, 0, 0, 0, 0, 0, 1}}, .tile_attack_interaction = pipe_seam_attack_interaction})
        {
        }

        int getPotentialDamageFrom(const Unit &unit) const override
        {
            const bool can_unit_attack_with_primary = unit.ammo > 0 && damage_table[unit.id].primary;
            auto damage{0};
            if (can_unit_attack_with_primary)
                damage = *damage_table[unit.id].primary;
            else
                damage = *damage_table[unit.id].secondary;

            return (damage * unit.getUnitCount()) / 10;
        }

        std::optional<int> getHealth() const override
        {
            return health;
        }

        int health{99};
    };
}

bool PipeSeamAttackInteraction::isAttackable(Map &map, const TileIndex &index, const Unit &unit) const
{
    const bool can_unit_attack_with_primary = unit.ammo > 0 && damage_table[unit.id].primary.has_value();
    const bool can_unit_attack_with_secondary = damage_table[unit.id].secondary.has_value();

    const bool can_unit_attack_pipe_seam = can_unit_attack_with_primary || can_unit_attack_with_secondary;

    const Tiles::PipeSeam &pipe_seam = (Tiles::PipeSeam &)map[index];
    return can_unit_attack_pipe_seam && pipe_seam.health > 0;
}

void PipeSeamAttackInteraction::attack(Map &map, const TileIndex &unit_tile, const TileIndex &tile) const
{
    const auto &unit = map[unit_tile].unit;
    const bool can_unit_attack_with_primary = unit->ammo > 0 && damage_table[unit->id].primary;
    Tiles::PipeSeam &pipe_seam = (Tiles::PipeSeam &)map[tile];

    auto damage{0};
    if (can_unit_attack_with_primary)
    {
        map[unit_tile].unit->ammo--;
        damage = *damage_table[unit->id].primary;
    }
    else
    {
        damage = *damage_table[unit->id].secondary;
    }

    pipe_seam.health -= (damage * unit->getUnitCount()) / 10;
    if (pipe_seam.health > 0)
        return;

    pipe_seam.info.movement_costs = MovementCosts{{1, 1, 1, 2, 0, 0, 1, 0}};

    if (pipe_seam.direction == Tile::V)
        pipe_seam.setDirection(Tile::L);
    else
        pipe_seam.setDirection(Tile::U);

    map.animations.emplace_back(Animation{UI::GIF("misc/explosion/", "resources/images/"), 0.81, tile, 4, 8});
}

namespace
{
    bool init = init_tile<Tiles::PipeSeam>(113, Tile::Direction::V) &&
                init_tile<Tiles::PipeSeam>(114, Tile::Direction::H);
}

#include <game/tile.hpp>
#include <game/map.hpp>
#include <game/game.hpp>

namespace
{
    static ProduceUnit produce_unit{{"Infantry", "TransportCopter"}};
    static RepairUnits repair_units{};
}

namespace Tiles
{
    struct Airport : Tile
    {
        Airport() : Tile(TileInfo{.defense = 3, .movement_costs = MovementCosts{{1, 1, 1, 1, 0, 0, 1, 0}}, .player_interaction = produce_unit, .tile_round_behaviour = repair_units})
        {
        }
    };
}

namespace
{
    bool init = init_tile<Tiles::Airport>(45) &&
                init_tile<Tiles::Airport>(40, Red) &&
                init_tile<Tiles::Airport>(36, Blue);
}

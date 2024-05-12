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
    struct Base : Tile
    {
        Base() : Tile(TileInfo{.defense = 3, .movement_costs = MovementCosts{{1, 1, 1, 1, 0, 1, 1, 0}}, .player_interaction = produce_unit, .tile_round_behaviour=repair_units})
        {
        }
    };
}

namespace
{
    bool init = init_tile<Tiles::Base>(44) &&
                init_tile<Tiles::Base>(39, Red) &&
                init_tile<Tiles::Base>(35, Blue);
}

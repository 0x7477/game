#include <game/tile.hpp>
#include <game/map.hpp>
#include <game/game.hpp>


namespace
{
    static RepairUnits repair_units{};
}

namespace Tiles
{
    struct Headquarter : Tile
    {
        Headquarter() : Tile(TileInfo{.defense = 4, .movement_costs = MovementCosts{{1, 1, 1, 1, 0, 1, 1, 0}}, .tile_round_behaviour = repair_units})
        {
        }
    };
}

namespace
{
    bool init =
        init_tile<Tiles::Headquarter>(42, Red) &&
        init_tile<Tiles::Headquarter>(47, Blue);
}

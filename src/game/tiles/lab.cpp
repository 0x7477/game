#include <game/tile.hpp>
#include <game/map.hpp>
#include <game/game.hpp>


namespace Tiles
{
    struct Lab : Tile
    {
        Lab() : Tile(TileInfo{.defense = 3, .movement_costs =MovementCosts{{1, 1, 1, 1, 0, 1, 1, 0}}})
        {
        }
    };
}

namespace
{
    bool init = init_tile<Tiles::Lab>(145) &&
                init_tile<Tiles::Lab>(146, Red) &&
                init_tile<Tiles::Lab>(140, Blue);
}

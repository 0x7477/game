#include <game/tile.hpp>
#include <game/map.hpp>
#include <game/game.hpp>


namespace Tiles
{
    struct Port : Tile
    {
        Port() : Tile(TileInfo{.defense = 3, .movement_costs = MovementCosts{{1, 1, 1, 1, 0, 1, 1, 0}}})
        {
        }
    };
}

namespace
{
    bool init = init_tile<Tiles::Port>(46) &&
                init_tile<Tiles::Port>(41, Red) &&
                init_tile<Tiles::Port>(37, Blue);
}

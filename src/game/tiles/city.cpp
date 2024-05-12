#include <game/tile.hpp>
#include <game/map.hpp>
#include <game/game.hpp>


namespace Tiles
{
    struct City : Tile
    {
        City() : Tile(TileInfo{.defense = 3, .movement_costs = MovementCosts{{1, 1, 1, 1, 0, 1, 1, 0}}})
        {
        }
    };
}

namespace
{
    bool init = init_tile<Tiles::City>(43) &&
                init_tile<Tiles::City>(38, Red) &&
                init_tile<Tiles::City>(34, Blue);
}

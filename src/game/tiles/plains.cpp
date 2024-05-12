

#include <game/tile.hpp>

namespace Tiles
{
    struct Plains : Tile
    {
        Plains()
            : Tile({1, MovementCosts{{1, 1, 1, 2, 0, 0, 1, 0}}})
        {
        }
    };
}
namespace
{
    bool init = init_tile<Tiles::Plains>(1);
}

#include <game/tile.hpp>

namespace Tiles
{
    struct Woods : Tile
    {
        Woods()
            : Tile({2, MovementCosts{{1, 1, 2,3, 0, 0, 1, 0}}})
        {
        }
    };
}
namespace
{
    bool init = init_tile<Tiles::Woods>(3);
}

#include <game/tile.hpp>

namespace Tiles
{
    struct Mountains : Tile
    {
        Mountains()
            : Tile({4, MovementCosts{{2, 1, 0, 0, 0, 0, 1, 0}}})
        {
        }
    };
}
namespace
{
    bool init = init_tile<Tiles::Mountains>(2);
}

#include <game/tile.hpp>

namespace Tiles
{
    struct Sea : Tile
    {
        Sea()
            : Tile({0, {0, 0, 0,0, 1, 1, 1, 0}})
        {
        }
    };
}
namespace
{
    bool init = init_tile<Tiles::Sea>(28);
}

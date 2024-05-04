#include <game/tile.hpp>

namespace Tiles
{
    struct Reefs : Tile
    {
        Reefs()
            : Tile({1, {0, 0, 0, 2, 2, 1, 0}})
        {
        }
    };
}
namespace
{
    bool init = init_tile<Tiles::Reefs>(33);
}

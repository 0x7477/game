#include <game/tile.hpp>

namespace Tiles
{
    struct MissileSilo : Tile
    {
        MissileSilo()
            : Tile({3, MovementCosts{{1, 1, 1, 1, 0, 0, 1, 0}}})
        {
        }
    };
}
namespace
{
    bool init = init_tile<Tiles::MissileSilo>(111, Tile::D) &&
                init_tile<Tiles::MissileSilo>(112, Tile::U);
}

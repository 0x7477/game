#include <game/tile.hpp>

namespace Tiles
{
    struct Shoals : Tile
    {
        Shoals()
            : Tile({0, MovementCosts{{1, 1, 1,1, 0, 1, 1, 0}}})
        {
        }
    };
}
namespace
{
    bool init = init_tile<Tiles::Shoals>(29, Tile::Direction::U) &&
    init_tile<Tiles::Shoals>(30, Tile::Direction::D) &&
    init_tile<Tiles::Shoals>(31, Tile::Direction::R) &&
    init_tile<Tiles::Shoals>(32, Tile::Direction::L);
}

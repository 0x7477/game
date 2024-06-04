#include <game/tile.hpp>

namespace Tiles
{
    struct River : Tile
    {
        River()
            : Tile({0, MovementCosts{{2, 1, 0, 0, 0, 0, 1, 0}}})
        {
        }
    };
}
namespace
{
    bool init = init_tile<Tiles::River>(4, Tile::H) &&
                init_tile<Tiles::River>(5, Tile::V) &&
                init_tile<Tiles::River>(6, Tile::URDL) &&
                init_tile<Tiles::River>(7, Tile::RD) &&
                init_tile<Tiles::River>(8, Tile::DL) &&
                init_tile<Tiles::River>(9, Tile::LU) &&
                init_tile<Tiles::River>(10, Tile::UR) &&
                init_tile<Tiles::River>(11, Tile::RDL) &&
                init_tile<Tiles::River>(12, Tile::DLU) &&
                init_tile<Tiles::River>(13, Tile::LUR) &&
                init_tile<Tiles::River>(14, Tile::URD);
}

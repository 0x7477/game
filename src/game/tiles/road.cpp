#include <game/tile.hpp>

namespace Tiles
{
    struct Road : Tile
    {
        Road()
            : Tile({0, {1, 1, 1, 0, 0, 1, 0}})
        {
        }
    };
}
namespace
{
    bool init = init_tile<Tiles::Road>(15, Tile::H) &&
        init_tile<Tiles::Road>(16, Tile::V) &&
        init_tile<Tiles::Road>(17, Tile::URDL) &&
        init_tile<Tiles::Road>(18, Tile::RD) &&
        init_tile<Tiles::Road>(19, Tile::DL) &&
        init_tile<Tiles::Road>(20, Tile::LU) &&
        init_tile<Tiles::Road>(21, Tile::UR) &&
        init_tile<Tiles::Road>(22, Tile::RDL) &&
        init_tile<Tiles::Road>(23, Tile::DLU) &&
        init_tile<Tiles::Road>(24, Tile::LUR) &&
        init_tile<Tiles::Road>(25, Tile::URD) &&
        init_tile<Tiles::Road>(26, Tile::L) &&
        init_tile<Tiles::Road>(27, Tile::U);
}

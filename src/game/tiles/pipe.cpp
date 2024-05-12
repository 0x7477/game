#include <game/tile.hpp>

namespace Tiles
{
    struct Pipe : Tile
    {
        Pipe()
            : Tile({0, MovementCosts{{0, 0, 0, 0, 0, 0, 0, 1}}})
        {
        }
    };
}
namespace
{
    bool init = init_tile<Tiles::Pipe>(101, Tile::Direction::V) &&
                init_tile<Tiles::Pipe>(102, Tile::Direction::H) &&
                init_tile<Tiles::Pipe>(103, Tile::Direction::UR) &&
                init_tile<Tiles::Pipe>(104, Tile::Direction::RD) &&
                init_tile<Tiles::Pipe>(105, Tile::Direction::DL) &&
                init_tile<Tiles::Pipe>(106, Tile::Direction::LU) &&
                init_tile<Tiles::Pipe>(107, Tile::Direction::U) &&
                init_tile<Tiles::Pipe>(108, Tile::Direction::R) &&
                init_tile<Tiles::Pipe>(109, Tile::Direction::D) &&
                init_tile<Tiles::Pipe>(110, Tile::Direction::L);

}

#include <game/tile.hpp>

namespace Tiles
{
    struct Shoals : Tile
    {
        Shoals()
            : Tile({0, MovementCosts{{1, 1, 1,1, 0, 1, 1, 0}}})
        {
        }


        void updateBasedByNeighbors(std::array<Tile*, 8>& tiles) override
        {
            unsigned char bytes = 0;

            for(int i = 0; i < 8; i++)
            {
                if(!tiles[i]) continue;

                const auto tile_id = tiles[i]->getId();

                bytes |= (tile_id == "Sea" || tile_id == "Shoals" || tile_id == "Reefs" )?0: 1 << i; 
            }
            
            setTexture(std::to_string(bytes));
        }
    };
}
namespace
{
    bool init = init_tile<Tiles::Shoals>(29) &&
    init_tile<Tiles::Shoals>(30) &&
    init_tile<Tiles::Shoals>(31) &&
    init_tile<Tiles::Shoals>(32);
}

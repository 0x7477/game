#include <game/tile.hpp>

namespace Tiles
{
    struct Sea : Tile
    {
        Sea()
            : Tile({0, MovementCosts{{0, 0, 0,0, 1, 1, 1, 0}}})
        {
        }

        void updateBasedByNeighbors(std::array<Tile*, 8>& tiles) override
        {
            unsigned char bytes = 0;

            // bool has_shoals = false;

            for(int i = 0; i < 8; i++)
            {
                if(!tiles[i]) continue;

                const auto tile_id = tiles[i]->getId();

                // if(tile_id == "Shoals")
                //     has_shoals = true;
                bytes |= (tile_id == "Sea" || tile_id == "Shoals" || tile_id == "Reefs" )?0: 1 << i; 
            }

            setTexture(std::to_string(bytes));

            // if(has_shoals && bytes != 0)
            // {
            //     unsigned char shoal_bytes = 0;
            //     for(int i = 0; i < 8; i++)
            //     {
            //         if(!tiles[i]) continue;
            //         shoal_bytes |= (tiles[i]->getId() == "Shoals")?0: 1 << i; 
            //     }
            //     setTexture(std::to_string(bytes) + "_" + std::to_string(shoal_bytes));
            // }

        }
    };
}
namespace
{
    bool init = init_tile<Tiles::Sea>(28);
}

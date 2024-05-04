#include <game/tile.hpp>
#include <game/map.hpp>
#include <game/game.hpp>

static ProduceUnit produce_unit{{"Infantry", "TransportCopter"}};

namespace Tiles
{
    struct Base : Tile
    {
        Base() : Tile(TileInfo{.defense = 3, .movement_costs = {1, 1, 1, 1, 0, 1, 1, 0}, .player_interaction = produce_unit})
        {
        }
    };
}

namespace
{
    bool init = init_tile<Tiles::Base>(35) &&
                init_tile<Tiles::Base>(39, Red) &&
                init_tile<Tiles::Base>(44, Blue);
}

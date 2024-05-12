#include <game/tile.hpp>
#include <game/map.hpp>
#include <game/game.hpp>


namespace Tiles
{
    struct CommunicationTower : Tile
    {
        CommunicationTower() : Tile(TileInfo{.defense = 3, .movement_costs = MovementCosts{{1, 1, 1, 1, 0, 1, 1, 0}}})
        {
        }
    };
}

namespace
{
    bool init = init_tile<Tiles::CommunicationTower>(129) &&
                init_tile<Tiles::CommunicationTower>(134, Red) &&
                init_tile<Tiles::CommunicationTower>(133, Blue);
}

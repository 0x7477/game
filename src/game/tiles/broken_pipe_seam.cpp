#include <game/tile.hpp>
#include <game/map.hpp>
#include <map>
#include <game/attack_simulator.hpp>
#include <game/unit_names.hpp>


namespace Tiles
{
    class BrokenPipeSeam : public Tile
    {
    public:
        BrokenPipeSeam()
            : Tile({.defense = 0, .movement_costs = MovementCosts{{1, 1, 1, 2, 0, 0, 1, 0}}})
        {
        }
    };
}


namespace
{
    bool init = init_tile<Tiles::BrokenPipeSeam>(115, Tile::Direction::L) &&
                init_tile<Tiles::BrokenPipeSeam>(116, Tile::Direction::U);
}

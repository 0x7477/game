#include <game/unit.hpp>
#include <game/map.hpp>
#include <game/game.hpp>
#include <game/units/transport.hpp>
#include <game/unit_names.hpp>
namespace Units
{
    class Battleship : public Unit
    {

    public:
        Battleship(const Team &team)
            : Unit{team, {5,Ship,9,2,6}}
        {}
    };
}

namespace
{
    volatile bool init = init_unit<Units::Battleship>();
}
#include <game/unit.hpp>
#include <game/map.hpp>
#include <game/game.hpp>
#include <game/units/transport.hpp>
#include <game/unit_names.hpp>
namespace Units
{
    class Battlecopter : public Unit
    {

    public:
        Battlecopter(const Team &team)
            : Unit{team, {6,Air,6}}
        {}
    };
}

namespace
{
    volatile bool init = init_unit<Units::Battlecopter>();
}
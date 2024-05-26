#include <game/unit.hpp>
#include <game/map.hpp>
#include <game/game.hpp>
#include <game/units/transport.hpp>
#include <game/unit_names.hpp>
namespace Units
{
    class Bomber : public Unit
    {

    public:
        Bomber(const Team &team)
            : Unit{team, {7,Air,9}}
        {}
    };
}

namespace
{
    volatile bool init = init_unit<Units::Bomber>();
}
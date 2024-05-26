#include <game/unit.hpp>
#include <game/map.hpp>
#include <game/game.hpp>
#include <game/units/transport.hpp>
#include <game/unit_names.hpp>
namespace Units
{
    class Fighter : public Unit
    {

    public:
        Fighter(const Team &team)
            : Unit{team, {9,Air,9}}
        {}
    };
}

namespace
{
    volatile bool init = init_unit<Units::Fighter>();
}
#include <game/unit.hpp>
#include <game/map.hpp>
#include <game/game.hpp>
#include <game/units/transport.hpp>
#include <game/unit_names.hpp>
namespace Units
{
    class Neotank : public Unit
    {

    public:
        Neotank(const Team &team)
            : Unit{team, {6,Treads,9}}
        {}
    };
}

namespace
{
    volatile bool init = init_unit<Units::Neotank>();
}
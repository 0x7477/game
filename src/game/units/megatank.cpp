#include <game/unit.hpp>
#include <game/map.hpp>
#include <game/game.hpp>
#include <game/units/transport.hpp>
#include <game/unit_names.hpp>
namespace Units
{
    class Megatank : public Unit
    {

    public:
        Megatank(const Team &team)
            : Unit{team, {4,Treads,3}}
        {}
    };
}

namespace
{
    volatile bool init = init_unit<Units::Megatank>();
}
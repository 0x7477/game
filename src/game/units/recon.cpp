#include <game/unit.hpp>
#include <game/map.hpp>
#include <game/game.hpp>
#include <game/units/transport.hpp>
#include <game/unit_names.hpp>
namespace Units
{
    class Recon : public Unit
    {

    public:
        Recon(const Team &team)
            : Unit{team, {8,Tires,0}}
        {}
    };
}

namespace
{
    volatile bool init = init_unit<Units::Recon>();
}
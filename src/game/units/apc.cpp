#include <game/unit.hpp>
#include <game/units/transport.hpp>
namespace Units
{

    class APC : public Transport<1>
    {

    public:
        APC(const Team &team)
            : Transport{team, {6, Treads, 0}, {INFANTRY, MECH}}
        {
        }
    };
}

namespace
{
    volatile bool init = init_unit<Units::APC>();
}
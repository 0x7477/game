#include <game/units/transport.hpp>
namespace Units
{
    class TransportCopter : public Transport<1>
    {

    public:
        TransportCopter(const Team &team)
            : Transport{team, {6,Air,0},  {INFANTRY, MECH}}
        {}
    };
}

namespace
{
    volatile bool init = init_unit<Units::TransportCopter>();
}
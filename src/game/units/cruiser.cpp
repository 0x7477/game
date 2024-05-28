#include <game/units/transport.hpp>
#include <game/unit_names.hpp>
namespace Units
{
    class Cruiser : public Transport<2>
    {

    public:
        Cruiser(const Team &team)
            : Transport{team, {7,Ship,9}, {BATTLECOPTER, TCOPTER}}
        {}
    };
}

namespace
{
    volatile bool init = init_unit<Units::Cruiser>();
}
#include <game/unit.hpp>
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
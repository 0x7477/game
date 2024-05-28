#include <game/unit.hpp>
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
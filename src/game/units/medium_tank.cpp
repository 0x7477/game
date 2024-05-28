#include <game/unit.hpp>
namespace Units
{
    class MediumTank : public Unit
    {

    public:
        MediumTank(const Team &team)
            : Unit{team, {5,Treads,8}}
        {}
    };
}

namespace
{
    volatile bool init = init_unit<Units::MediumTank>();
}
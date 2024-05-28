#include <game/unit.hpp>
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
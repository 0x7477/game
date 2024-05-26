#include <game/units/trooper.hpp>
namespace Units
{
    class Infantry : public Trooper
    {
    public:
        Infantry(const Team &team)
            : Trooper{team, {3, Foot, 0}}
        {
        }
    };

}
namespace
{
    volatile bool init = init_unit<Units::Infantry>();
}
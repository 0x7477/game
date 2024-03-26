#include <spaceship_part.hpp>
#include <iostream>

namespace SpaceshipParts
{
    class Hull : public SpaceshipPart
    {
    public:
        Hull()
        {
        }

    };

    static auto init = []()
    {
        SpaceshipParts::Hull::registerClass<SpaceshipParts::Hull>();
        return true;
    }();
}



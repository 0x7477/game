#include "component.hpp"
#include "game_object.hpp"


Component::Component(GameObject *game_object)
    : game_object{game_object}
{
}
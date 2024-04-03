#include <engine/component.hpp>
#include <engine/game_object.hpp>

Component::Component(GameObject *game_object)
    : game_object{game_object}
{
}
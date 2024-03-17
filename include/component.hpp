#pragma once

class GameObject;
class Component
{
    public:
    Component(GameObject* game_object);


    GameObject* game_object;
};
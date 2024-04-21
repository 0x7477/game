#pragma once
#include <engine/component.hpp>
#include <vector>
#include <SFML/Graphics.hpp>
#include <chrono>
class Script;


namespace Scripts
{
    void update();
    void display(sf::RenderWindow& window);
    static std::vector<Script*> scripts;
}

class Script: public Component
{
    public:
    Script(GameObject* game_object);

    virtual void update(const float& delta_time);
    virtual void display(sf::RenderWindow& window);

};
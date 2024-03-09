#pragma once
#include <vector>
#include <SFML/Graphics.hpp>

class GameObject
{
public:
    GameObject(const bool &controllable = true)
    {
        game_objects.push_back(this);
        if (controllable)
            controllable_game_objects.push_back(this);
    }

    virtual void update(sf::RenderWindow &window){};
    virtual void display(sf::RenderWindow &window){};


    static void update();

    private:

    static inline std::vector<GameObject *> game_objects{};
    static inline std::vector<GameObject *> controllable_game_objects{};

};

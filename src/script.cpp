#include "script.hpp"
#include "game_object.hpp"
#include <iostream>

Script::Script(GameObject *game_object)
    : Component{game_object}
{
    Scripts::scripts.push_back(this);
}

void Script::update(const double& delta_time){}
void Script::display(sf::RenderWindow& window){}

double getDeltaTime()
{
    static bool first_delta_time = true;
    static auto last_update = std::chrono::system_clock::now();
    if(first_delta_time)
    {
        first_delta_time = false;   
        last_update = std::chrono::system_clock::now();
        return 0;
    }

    const auto current_time = std::chrono::system_clock::now();
    std::chrono::duration<double> delta_time = current_time - last_update;
    last_update = current_time;

    return delta_time.count();
}

void Scripts::update()
{
    const auto delta_time = getDeltaTime();
    for(auto& script: Scripts::scripts)
        script->update(delta_time);
}

void Scripts::display(sf::RenderWindow& window)
{
    for(auto& script: Scripts::scripts)
        script->display(window);
}



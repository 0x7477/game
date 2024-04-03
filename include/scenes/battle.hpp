#pragma once
#include <engine/synced.hpp>
#include <game/player.hpp>
#include <network/network_manager.hpp>
#include <SFML/Graphics.hpp>

#include <engine/scene.hpp>
#include <game/map.hpp>
namespace Scene
{

class Battle : public Scene
{
    public:
    Battle(WindowManager &window_manager, const std::string& map_data);
    virtual void run() override;

private:
    sf::RenderWindow window;
    Map map;
};

}


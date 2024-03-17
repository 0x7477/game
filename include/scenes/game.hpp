#pragma once
#include "synced.hpp"
#include "player.hpp"
#include "network/network_manager.hpp"
#include <SFML/Graphics.hpp>

#include "scene.hpp"

namespace Scene
{

class Game : public Scene
{
    public:
    Game(WindowManager &window_manager,network::NetworkManager& network_manager);
    virtual void run() override;

private:
    network::NetworkManager& network_manager;

    sf::RenderWindow window;
};

}


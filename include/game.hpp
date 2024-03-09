#pragma once
#include "network/synced.hpp"
#include "player.hpp"
#include "network/network_manager.hpp"
#include <SFML/Graphics.hpp>


class Game
{
    public:
    Game(network::NetworkManager& network_manager);
    void run();

    static bool keys[sf::Keyboard::KeyCount];

    private:
    network::NetworkManager& network_manager;

    sf::RenderWindow window;
};



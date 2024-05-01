#pragma once
#include <engine/synced.hpp>
#include <game/player.hpp>
#include <network/network_manager.hpp>
#include <SFML/Graphics.hpp>

#include <engine/scene.hpp>
#include <game/map.hpp>
#include <network/network_manager.hpp>
namespace Scene
{

    class Battle : public Scene
    {
    public:
        Battle(WindowManager &window_manager, network::NetworkManager &network_manager, const std::string &map_data);
        virtual void run() override;

        void sendMoveUnit(const std::vector<TileIndex>& path);

    private:
        sf::RenderWindow window;
        network::NetworkManager &network_manager;

    public:
        Map map;
    };

}

inline Scene::Battle* battle_scene;
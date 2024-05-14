#include <scenes/battle.hpp>
#include <game/map.hpp>
#include <game/unit.hpp>

Scene::Battle::Battle(WindowManager &window_manager, network::NetworkManager &network_manager, const std::string &map_data)
    : Scene{"map", window_manager}, network_manager{network_manager}, game{network_manager, map_data, *this}
{
    battle_scene = this;
}


void Scene::Battle::run()
{
    auto &window = window_manager.window;
    window.clear(sf::Color(35, 45, 55));

    game.map.display(window);
    window.display();
}

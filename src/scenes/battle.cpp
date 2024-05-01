#include <scenes/battle.hpp>
#include <game/map.hpp>

Scene::Battle::Battle(WindowManager &window_manager, network::NetworkManager &network_manager, const std::string& map_data)
    : Scene{"map", window_manager}, network_manager{network_manager}, map{map_data, Red}
{
    battle_scene = this;
}

network::RPC<network::Datagram<std::vector<TileIndex>>, [](const auto &data)
{
    const auto tiles = data.get();
    battle_scene->map.moveUnit(*tiles.begin(), *tiles.end());
}>
move_unit;



void Scene::Battle::run()
{
    auto &window = window_manager.window;
    window.clear(sf::Color(35, 45, 55));

    map.display(window);
    window.display();
}

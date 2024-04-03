#include <scenes/battle.hpp>

Scene::Battle::Battle(WindowManager &window_manager, const std::string& map_data)
    : Scene{"map", window_manager}, map{map_data}
{
}



void Scene::Battle::run()
{
    auto &window = window_manager.window;
    window.clear(sf::Color(35, 45, 55));

    map.display(window);
    window.display();
}

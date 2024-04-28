#include <scenes/menu.hpp>
#include <iostream>

Scene::Menu::Menu(WindowManager &window_manager, network::NetworkManager &network_manager,Lobby& lobby)
    : Scene{"menu", window_manager},
      network_manager{network_manager},
      lobby{lobby},
      title_font{font_resources.get("arial.ttf")}
{
    play_button.setColor(sf::Color::Blue);
    editor_button.setColor(sf::Color::Green);
    exit_button.setColor(sf::Color::Red);
}

void Scene::Menu::createGame()
{
}

void Scene::Menu::run()
{
    auto &window = window_manager.window;

    window.clear(sf::Color(35, 45, 55));
    play_button.draw(window);
    editor_button.draw(window);
    exit_button.draw(window);
    window.display();
}

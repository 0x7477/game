#include <scenes/menu.hpp>
#include <iostream>

Scene::Menu::Menu(WindowManager &window_manager, network::NetworkManager &network_manager,Lobby& lobby)
    : Scene{"menu", window_manager},
      network_manager{network_manager},
      lobby{lobby},
      title_font{font_resources.get("arial.ttf")}, background{image_resources.get("misc/startscreen.png")}
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

    background.setScale((float)WindowManager::window_width / background.getTexture()->getSize().x,(float) WindowManager::window_height / background.getTexture()->getSize().y);
    window.draw(background);

    play_button.draw(window);
    editor_button.draw(window);
    exit_button.draw(window);
    window.display();
}

#include <scenes/game.hpp>
#include <iostream>
#include <network/rpc.hpp>
#include <game/player.hpp>

Scene::Game::Game(WindowManager &window_manager, network::NetworkManager &network_manager)
    : Scene{"game", window_manager},
      network_manager{network_manager}
{
}


void Scene::Game::run()
{
    GameObject player;
    player.addComponent<Synced>(network::RPCTargets::AllClientsBuffered);

    auto& synced = player.getComponent<Synced>();
    synced.addComponent<Player>(std::string("Peter"));
    synced.execute(print_rpc);
    synced.execute(set_health_rpc, 5);

    
    // while (window.isOpen())
    // {
    //     updateKeys();
    //     window.clear(sf::Color::Blue);

    //     Scripts::update();
    //     Scripts::display(window);


    //     // sf::Text text{};
    //     // sf::Font font;
    //     // if (!font.loadFromFile("resources/fonts/arial.ttf"))
    //     // {
    //     //     // Error handling if the font file couldn't be loaded
    //     //     // return EXIT_FAILURE;
    //     // }

    //     // text.setFont(font);
    //     // text.setCharacterSize(100);
    //     // text.setString("hallo");
    //     // text.setPosition(200, 200);
    //     // text.setFillColor(sf::Color::White);

    //     // window.draw(text);

    //     window.display();
    
}

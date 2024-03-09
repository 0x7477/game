#include <game.hpp>
#include <iostream>
#include <network/rpc.hpp>
#include <player.hpp>

bool Game::keys[sf::Keyboard::KeyCount]{};


Game::Game(network::NetworkManager &network_manager)
    : network_manager{network_manager},
      window(sf::VideoMode(800, 600), "SFML Window")
{
}

void Game::run()
{
    network::Synced<Player> player{network::RPCTargets::AllClientsBuffered, std::string("Peter")};

    player.execute(print_rpc);
    player.execute(set_health_rpc, 5);
    player.execute(print_rpc);

    while (window.isOpen()) 
    {
        // Handle events
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
                window.close(); // Close the window when Escape key is pressed

            if(event.type == sf::Event::KeyPressed)
                keys[event.key.code] = true;

            if(event.type == sf::Event::KeyReleased)
                keys[event.key.code] = false;

        }

        window.clear(sf::Color::Blue);

        for(const auto& game_object :controllable_game_objects)
        {
            game_object->update(window);
            game_object->display(window);
        }

        sf::Text text{};
            sf::Font font;
    if (!font.loadFromFile("resources/fonts/arial.ttf")) 
    {
        // Error handling if the font file couldn't be loaded
        // return EXIT_FAILURE;
    }

        text.setFont(font);
        text.setCharacterSize(100);
        text.setString("hallo");
        text.setPosition(200,200);
        text.setFillColor(sf::Color::White);

        window.draw(text);

        window.display();
    }
}

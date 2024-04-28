#include <scenes/lobby.hpp>
#include <iostream>
#include <map>
#include <game/lobby.hpp>
#include <random>

std::string generateRandomId()
{
    const std::string charset = "0123456789";
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, charset.size() - 1);

    std::string randomId;
    for (int i = 0; i < 5; ++i)
        randomId += charset[dis(gen)];
    return randomId;
}

std::string Scene::Lobby::getRandomPlayerName()
{
    std::array<std::string> names = "Karl, Peter, Katha, Vero";
    return names[names.size() % network_manager.getID()];
}

network::RPC<network::Datagram<unsigned, std::string>, [](const auto &data)
             {
                 std::string id;
                 while (true)
                 {
                     id = generateRandomId();
                     if (!lobbies.contains(id))
                     {
                         std::cout << id << "\n";
                         lobbies[id] = Lobby{id};

                         std::string name = std::get<std::string>(data.getData());
                         std::string player_id = std::get<std::string>(data.getData());

                         lobbies[id].addPlayer({name, player_id});
                         break;
                     }
                 }
                 return network::Datagram<std::string>{id};
             },
             network::RPC<network::Datagram<std::string>, [](const auto &data)
                          {
                              const std::string lobby_id = data.get();
                              lobby_scene->setLobbyId(lobby_id);
                          }>>
    create_game;

network::RPC<network::Datagram<unsigned>, [](const auto &data)
             {
                 lobby_scene->notifyPlayerJoined(data.get());
             }>
    player_joined;

network::RPC<network::Datagram<std::string, unsigned>, [](const auto &data)
             {
                 const auto tuple = data.getData();
                 const auto game_id = std::get<0>(tuple);
                 const auto player_id = std::get<1>(tuple);
                 if (!lobbies.contains(game_id))
                     return network::Datagram<bool, std::vector<unsigned>>{false, {}};

                 lobbies[game_id].addPlayer(player_id);

                 network::NetworkManager::send(player_joined, network::RPCTargets::AllClients, {player_id});

                 return network::Datagram<bool, std::vector<LobbyMember>>{true, lobbies[game_id].getLobbyMember()};
             },
             network::RPC<network::Datagram<bool, std::vector<LobbyMember>>, [](const auto &data)
                          {
                              const auto tuple = data.getData();
                              const auto success = std::get<0>(tuple);
                              if (!success)
                              {
                                  std::optional<std::string> opt{"failed to connect"};
                                  lobby_scene->onJoined(opt);
                                  return;
                              }

                              std::cout << "successfully joined\n";
                              const auto players = std::get<1>(tuple);

                              for (const auto &player : players)
                                  lobby.addPlayer(player);

                              std::optional<std::string> opt{};

                              lobby_scene->onJoined(opt);
                          }>>
    join_game;

Scene::Lobby::Lobby(WindowManager &window_manager, network::NetworkManager &network_manager)
    : Scene{"lobby", window_manager},
      network_manager{network_manager},
      title_text{"", font_resources.get("arial.ttf")},
      room_number{"", font_resources.get("arial.ttf")}
{
    lobby_scene = this;
    create_button.setColor(sf::Color::Blue);
    join_button.setColor(sf::Color::Blue);
    exit_button.setColor(sf::Color::Red);
    error_button.setColor(sf::Color::Red);

    room_number.setPosition(WindowManager::window_width * 0.2, WindowManager::window_height * 0.2);
}

void setReady(const unsigned id)
{
    std::cout << "set ready\n";
}
void Scene::Lobby::notifyPlayerJoined(const unsigned &id)
{
    if (id == network_manager.getID())
        return;
    lobby.addPlayer(id);
    std::cout << id << " joined\n";
}

void Scene::Lobby::displayJoinError(const std::string &error_)
{
    error = error_;
}

void Scene::Lobby::setLobbyId(const network::Datagram<std::string> &data)
{
    std::cout << "created room\n";
    id = data.get();
    room_number.setString(id);
    joined = true;

    lobby.addPlayer(network_manager.getID());
}

void Scene::Lobby::drawLobby()
{
    auto &window = window_manager.window;
    window.clear(sf::Color(35, 45, 55));
    window.draw(room_number);

    unsigned player_number{0};
    for (const auto &player : lobby.players)
    {
        room_number.setString(std::to_string(player));
        room_number.setPosition(200, 200 + player_number * 30);
        window.draw(room_number);
    }

    create_button.draw(window);
    exit_button.draw(window);
    window.display();
}

void Scene::Lobby::joinGame()
{
    if (error)
        return;
    const auto game_id = lobby_id_input.getValue();
    std::cout << "try to join " << game_id << "\n";
    network_manager.send(join_game, network::RPCTargets::Server, {game_id, network_manager.getID()});
}

void Scene::Lobby::promptCode()
{
    auto &window = window_manager.window;
    window.clear(sf::Color(35, 45, 55));
    window.draw(room_number);

    if (error)
        error_button.draw(window);
    else
    {
        join_button.draw(window);
        exit_button.draw(window);
        lobby_id_input.draw(window);
    }

    window.display();
}

void Scene::Lobby::run()
{
    if (joined)
    {
        drawLobby();
        return;
    }

    if (mode == Create && !connecting)
    {
        network_manager.send(create_game, network::RPCTargets::Server, {network_manager.getID(), getRandomPlayerName()});
        connecting = true;
        return;
    }
    if (mode == Join && !joined)
    {
        promptCode();
        return;
    }
    if (mode == Join && joined)
    {
        drawLobby();
        return;
    }
}

void Scene::Lobby::setId(const std::string &id_)
{
    id = id_;
}

void Scene::Lobby::setMode(const Mode &mode_)
{
    mode = mode_;
}

void Scene::Lobby::onJoined(const std::optional<std::string> &error_)
{
    if (error_)
    {
        error = error_;
        error_button.setString(*error);
    }
    else
    {
        for (const auto &player : lobby.players)
            std::cout << "player " << player << " in lobby\n";
        joined = true;
    }
}
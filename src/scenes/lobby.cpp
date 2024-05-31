#include <scenes/lobby.hpp>
#include <iostream>
#include <map>
#include <game/lobby.hpp>
#include <random>
#include <array>
#include <unistd.h>
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

std::string Scene::Lobby::getPlayerName()
{
    return getlogin();
}

network::RPC<network::Datagram<LobbyMember>, [](const auto &data)
             {
                 std::string id;
                 while (true)
                 {
                     id = generateRandomId();
                     if (!lobbies.contains(id))
                     {
                         std::cout << id << "\n";
                         lobbies[id] = Lobby{id};
                         lobbies[id].addPlayer(data.get());
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
    create_game{"create_game"};

network::RPC<network::Datagram<LobbyMember>, [](const auto &data)
             {
                 lobby_scene->notifyPlayerJoined(data.get());
             }>
    player_joined{"player_joined"};
network::RPC<network::Datagram<unsigned>, [](const auto &data)
             {
                 lobby_scene->setReady(data.get());
             }>
    player_ready{"player_ready"};

network::RPC<network::Datagram<std::string, LobbyMember>, [](const auto &data)
             {
                 const auto tuple = data.getData();
                 const auto game_id = std::get<0>(tuple);
                 const auto lobby_member = std::get<1>(tuple);

                 if (!lobbies.contains(game_id))
                     return network::Datagram<bool, std::vector<LobbyMember>>{false, {}};

                 if (lobbies[game_id].getNumberOfPlayers() >= 2)
                     return network::Datagram<bool, std::vector<LobbyMember>>{false, {}};

                 lobbies[game_id].addPlayer(lobby_member);

                 network::NetworkManager::send(player_joined, network::RPCTargets::AllClients, {lobby_member});
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
                              const std::vector<LobbyMember> players = std::get<1>(tuple);

                              for (const auto &player : players)
                                  lobby.addPlayer(player);

                              std::optional<std::string> opt{};

                              lobby_scene->onJoined(opt);
                          }>>
    join_game{"join_game"};

Scene::Lobby::Lobby(WindowManager &window_manager, network::NetworkManager &network_manager, Battle &battle)
    : Scene{"lobby", window_manager},
      network_manager{network_manager},
      battle{battle},
      title_text{"", font_resources.get("arial.ttf")},
      room_number{"", font_resources.get("arial.ttf")}
{
    lobby_scene = this;
    if (!ready)
        ready_button.setColor(sf::Color::Blue);
    join_button.setColor(sf::Color::Blue);
    exit_button.setColor(sf::Color::Red);
    error_button.setColor(sf::Color::Red);

    room_number.setPosition(WindowManager::window_width * 0.2, WindowManager::window_height * 0.2);
}

void Scene::Lobby::setReady(const unsigned &id)
{
    lobby.players[id].ready = true;

    if (lobby.areAllMembersReady())
    {
        battle.game.commitLobby(lobby);
        battle.game.map.beginTurn(Team::Red);
        setScene("map");
    }
}

void Scene::Lobby::sendReady()
{
    std::cout << "send Ready\n";
    network_manager.send(player_ready, network::RPCTargets::AllClients, {network_manager.getID()});
}
void Scene::Lobby::notifyPlayerJoined(const LobbyMember &member)
{
    if (member.network_id == network_manager.getID())
        return;
    lobby.addPlayer(member);
    std::cout << id << " joined\n";
}

void Scene::Lobby::displayJoinError(const std::string &error_)
{
    error = error_;
}

void Scene::Lobby::setLobbyId(const network::Datagram<std::string> &data)
{
    id = data.get();
    std::cout << "created room" << id << "\n";
    room_number.setString(id);
    joined = true;
    battle.game.map.team = Team::Red;

    lobby.addPlayer({username, network_manager.getID(), Team::Red});
}

void Scene::Lobby::drawLobby()
{
    auto &window = window_manager.window;
    window.clear(sf::Color(35, 45, 55));
    window.draw(room_number);

    unsigned player_number{0};
    for (const auto &[_, player] : lobby.players)
    {
        room_number.setString(player.name);
        room_number.setPosition(200, 200 + player_number * 30);
        room_number.setFillColor(player.ready ? sf::Color::White : sf::Color::Red);
        window.draw(room_number);
        player_number++;
    }

    if (!ready)
        ready_button.draw(window);
    exit_button.draw(window);
    window.display();
}

void Scene::Lobby::joinGame()
{
    if (error)
        return;
    const auto game_id = lobby_id_input.getValue();
    std::cout << "try to join " << game_id << "\n";
    network_manager.send(join_game, network::RPCTargets::Server, {game_id, LobbyMember{username, network_manager.getID(), Team::Blue}});
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
    if (username == "")
        username = getPlayerName();

    if (joined)
    {
        drawLobby();
        return;
    }

    if (mode == Create && !connecting)
    {
        network_manager.send(create_game, network::RPCTargets::Server, {LobbyMember{getPlayerName(), network_manager.getID(), Team::Red}});
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
        for (const auto &[id, _] : lobby.players)
            std::cout << "player " << id << " in lobby\n";
        joined = true;
        battle.game.map.team = Team::Blue;
    }
}
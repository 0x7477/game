#pragma once

#include <game/map.hpp>
#include <game/player.hpp>
#include <network/network_manager.hpp>
#include <game/lobby.hpp>
#include <map>

namespace Scene
{
    class Battle;
}

class Game
{
public:
    Game(network::NetworkManager &network_manager, const std::string &map_data, Scene::Battle& battle_scene);

    void commitLobby(const Lobby &lobby);

    void endTurn();
    void sendAction(const Unit::ActionId &id, const std::vector<TileIndex> &path, const TileIndex &target, const unsigned& index = 0);
    void sendCreateUnit(const std::string &unit_id, const Team &team, const TileIndex &target);

    network::NetworkManager &network_manager;

    std::map<Team, Player> players{};
    Team current_active_player{Red};
    Map map;
    Scene::Battle& battle_scene;

    static Game *game;

};

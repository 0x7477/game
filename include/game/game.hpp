#pragma once

#include <game/map.hpp>
#include <game/player.hpp>
#include <network/network_manager.hpp>
#include <game/lobby.hpp>
#include <map>

class Game
{
public:
    Game(network::NetworkManager &network_manager, const std::string &map_data)
        : network_manager{network_manager}, map{*this, map_data}
    {
        game = this;
    }

    void commitLobby(const Lobby &lobby)
    {
        for (const auto &[_, player] : lobby.players)
            players[player.team] = Player(player.name, player.team);
    }

    void endTurn();
    void sendAction(const Unit::ActionId &id, const std::vector<TileIndex> &path, const TileIndex &target);
    void sendCreateUnit(const std::string &unit_id, const Team &team, const TileIndex &target);

    network::NetworkManager &network_manager;

    std::map<Team, Player> players{};
    Map map;
    Team current_active_player{Red};

    static Game *game;
};

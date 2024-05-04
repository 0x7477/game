#pragma once

#include <game/map.hpp>
#include <game/player.hpp>
#include <network/network_manager.hpp>
#include <map>


class Game
{
public:
    Game(network::NetworkManager &network_manager, const std::string& map_data)
    :network_manager{network_manager}, map{*this,map_data}
    {
        game = this;
    }

    void sendAction(const Unit::ActionId& id, const std::vector<TileIndex>& path, const TileIndex& target);
    void sendCreateUnit(const std::string& unit_id, const Team& team, const TileIndex& target);


    network::NetworkManager &network_manager;
    std::map<unsigned, Player> players{};
    Map map;

    static Game* game;

};


#include <game.hpp>
#include <iostream>
#include <network/rpc.hpp>
#include <player.hpp>


Game::Game()
{
    network::Synced<Player> player{{"Peter"}};
    player.t.print();

    network::network_manager->send(print_rpc, network::RPCTarget::All, {player.player, player.id});
}

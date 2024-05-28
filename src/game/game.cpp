#include <network/datagram.hpp>
#include <network/rpc.hpp>
#include <game/unit.hpp>
#include <game/game.hpp>

Game *Game::game{nullptr};

    Game::Game(network::NetworkManager &network_manager, const std::string &map_data, Scene::Battle& battle_scene)
        : network_manager{network_manager}, map{*this, map_data}, battle_scene{battle_scene}
    {
        game = this;
    }

    void Game::commitLobby(const Lobby &lobby)
    {
        for (const auto &[_, player] : lobby.players)
            players[player.team] = Player(player.name, player.team);
    }

void unit_action(const network::Datagram<Unit::ActionId, std::vector<TileIndex>, TileIndex, unsigned> &data)
{
    const auto id = std::get<0>(data.getData());
    const auto path = std::get<1>(data.getData());
    const auto target = std::get<2>(data.getData());
    const auto index = std::get<3>(data.getData());

    // for (const auto &tile : path)
    //     std::cout << tile << ",";
    // std::cout << "\n";

    const auto path_start = *path.begin();
    const auto path_end = *(path.end() - 1);

    Game::game->map.moveMapToContain(path_start);
    Game::game->map.moveMapToContain(path_end);

    const auto &unit = Game::game->map[path_start].unit;

    unit->movement_manager.setPath(path);
    unit->movement_manager.startAnimation([unit, path_start, path_end, target, id, index]()
                                          { unit->executeAction(id, Game::game->map, path_start, path_end, target, index); });
}
network::RPC<network::Datagram<Unit::ActionId, std::vector<TileIndex>, TileIndex, unsigned>, unit_action> unit_action_rpc{"unit_action_rpc"};

void create_unit(const network::Datagram<std::string, Team, TileIndex> &data)
{
    const auto unit_id = std::get<0>(data.getData());
    const auto team = std::get<1>(data.getData());
    const auto target = std::get<2>(data.getData());

    Game::game->map.createUnit(unit_id, team, target);
    Game::game->map[target].unit->setFinished();
    Game::game->players[team].money -= Unit::unit_costs[unit_id];
}
network::RPC<network::Datagram<std::string, Team, TileIndex>, create_unit> create_unit_rpc{"create_unit_rpc"};

void Game::sendAction(const Unit::ActionId &id, const std::vector<TileIndex> &path, const TileIndex &target, const unsigned &index)
{
    network_manager.send(unit_action_rpc, network::RPCTargets::Clients, {id, path, target, index});
}

void Game::sendCreateUnit(const std::string &unit_id, const Team &team, const TileIndex &target)
{
    network_manager.send(create_unit_rpc, network::RPCTargets::Clients, {unit_id, team, target});
}

void end_turn(const network::Datagram<> &)
{
    Game::game->current_active_player = Game::game->current_active_player == Red ? Blue : Red;
    Game::game->map.endTurn();

    Game::game->map.beginTurn(Game::game->current_active_player);
}
network::RPC<network::Datagram<>, end_turn> end_turn_rpc{"end_turn_rpc"};

void Game::endTurn()
{
    network_manager.send(end_turn_rpc, network::RPCTargets::AllClients, {});
}

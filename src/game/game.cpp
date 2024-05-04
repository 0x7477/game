#include <network/datagram.hpp>
#include <network/rpc.hpp>
#include <game/unit.hpp>
#include <game/game.hpp>


Game* Game::game{nullptr};

void endTurn(const network::Datagram<> &data)
{

}

void unit_action(const network::Datagram<Unit::ActionId, std::vector<TileIndex>, TileIndex> &data)
{
    const auto id = std::get<0>(data.getData());
    const auto path = std::get<1>(data.getData());
    const auto target = std::get<2>(data.getData());

    const auto path_start = *path.begin();
    const auto path_end = *(path.end() - 1);

    const auto &unit = Game::game->map[path_start].unit;

    unit->movement_manager.setPath(path);
    unit->movement_manager.startAnimation([unit, path_start, path_end, target, id]()
    {
        unit->executeAction(id, Game::game->map, path_start, path_end, target);
    });

}
network::RPC<network::Datagram<Unit::ActionId, std::vector<TileIndex>, TileIndex>, unit_action> unit_action_rpc;

void create_unit(const network::Datagram<std::string, Team, TileIndex> &data)
{
    const auto unit_id = std::get<0>(data.getData());
    const auto team = std::get<1>(data.getData());
    const auto target = std::get<2>(data.getData());

    Game::game->map.createUnit(unit_id, team, target);
    Game::game->map[target].unit->setFinished();
}
network::RPC<network::Datagram<std::string, Team, TileIndex>, create_unit> create_unit_rpc;

void Game::sendAction(const Unit::ActionId &id, const std::vector<TileIndex> &path, const TileIndex &target)
{
    network_manager.send(unit_action_rpc, network::RPCTargets::Clients, {id, path, target});
}

void Game::sendCreateUnit(const std::string& unit_id, const Team& team, const TileIndex& target)
{
    network_manager.send(create_unit_rpc, network::RPCTargets::Clients, {unit_id, team, target});
}
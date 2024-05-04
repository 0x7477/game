#include <scenes/battle.hpp>
#include <game/map.hpp>
#include <game/unit.hpp>

Scene::Battle::Battle(WindowManager &window_manager, network::NetworkManager &network_manager, const std::string &map_data)
    : Scene{"map", window_manager}, network_manager{network_manager}, map{*this, map_data, Red}
{
    battle_scene = this;
}

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

    const auto &unit = battle_scene->map.getTile(*path.begin()).unit;

    unit->movement_manager.setPath(path);
    unit->movement_manager.startAnimation([unit, path_start, path_end, target, id]()
    {
        if (id == Unit::ActionId::Wait)
            unit->action<Unit::ActionId::Wait>(battle_scene->map, path_start, path_end, target);
        if (id == Unit::ActionId::Attack)
            unit->action<Unit::ActionId::Attack>(battle_scene->map, path_start, path_end, target);
    });

}
network::RPC<network::Datagram<Unit::ActionId, std::vector<TileIndex>, TileIndex>, unit_action> unit_action_rpc;

void create_unit(const network::Datagram<std::string, Team, TileIndex> &data)
{
    const auto unit_id = std::get<0>(data.getData());
    const auto team = std::get<1>(data.getData());
    const auto target = std::get<2>(data.getData());

    battle_scene->map.createUnit(unit_id, team, target);
    battle_scene->map[target].unit->setFinished();
}
network::RPC<network::Datagram<std::string, Team, TileIndex>, create_unit> create_unit_rpc;




void Scene::Battle::sendAction(const Unit::ActionId &id, const std::vector<TileIndex> &path, const TileIndex &target)
{
    network_manager.send(unit_action_rpc, network::RPCTargets::Clients, {id, path, target});
}

void Scene::Battle::sendCreateUnit(const std::string& unit_id, const Team& team, const TileIndex& target)
{
    network_manager.send(create_unit_rpc, network::RPCTargets::Clients, {unit_id, team, target});

}


void Scene::Battle::run()
{
    auto &window = window_manager.window;
    window.clear(sf::Color(35, 45, 55));

    map.display(window);
    window.display();
}

#include <engine/synced.hpp>

void construct(const network::Datagram<unsigned int, unsigned int, network::RPCTarget> &tuple)
{
    const auto &data = tuple.getData();
    const unsigned int &player = std::get<0>(data);
    const unsigned int &id{std::get<1>(data)};
    const network::RPCTarget &target{std::get<2>(data)};

    auto game_object = new GameObject();
    game_object->addComponent<Synced>(player, id, target);

    network::synced_objects[player].push_back(game_object);
}

Synced::Synced(GameObject* game_object, const network::RPCTarget &target)
    : Component(game_object), target{target}, owner{true}, player{network::NetworkManager::getID()}, id{(unsigned int)(network::synced_objects[player].size())}
{
    network::synced_objects[player].push_back(game_object);
    std::cout << "created synced class owner: " << owner << "\n";

    const network::Datagram<unsigned int, unsigned int, network::RPCTarget> datagram{player, id, target};

    network::NetworkManager::send(synced_constructor, target & ~network::RPCTargets::Me, datagram);
}

Synced::Synced(GameObject* game_object, const unsigned int &player, const unsigned int &id, const network::RPCTarget &target)
    : Component(game_object),target{target}, owner{false}, player{player}, id{id}
{
    std::cout << "created synced class owner: " << owner << "\n";        
}
#pragma once
#include <string>
#include <network/rpc.hpp>

struct LobbyMember
{
    LobbyMember(const LobbyMember& member) = default;
    LobbyMember(const std::string& name, const unsigned& id)
    :name{name}, network_id{id}
    {}

    static void serialize(std::vector<char> &buffer, const Player &value)
    {
        
    }

    static void deserialize(std::string_view &buffer, Player &value);

    std::string name{""};
    unsigned network_id{0};
    bool ready{false};
};

class Lobby
{
public:
    Lobby() = default;

    Lobby(const std::string& id)
    :id{id}
    {
        
    }

    std::size_t getNumberOfPlayers()
    {
        return players.size();
    }

    std::vector<LobbyMember> getLobbyMember()
    {
        std::vector<LobbyMember> member;

        for(const auto& [_, mem] : players)
            member.push_back(mem);
        return member;
    }
    void addPlayer(const LobbyMember& member)
    {
        players[member.network_id] = member;
    }
    
    std::vector<unsigned, LobbyMember> players{};
    std::string id;
};
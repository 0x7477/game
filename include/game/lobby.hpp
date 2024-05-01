#pragma once
#include <string>
#include <vector>
#include <map>
#include <network/rpc.hpp>
#include <game/teams.hpp>

class LobbyMember
{
public:
    LobbyMember() = default;
    LobbyMember(const LobbyMember &member) = default;
    LobbyMember(const std::string &name, const unsigned &id)
        : name{name}, network_id{id}
    {
    }

    static void serialize(std::vector<char> &buffer, const LobbyMember &value)
    {
        network::serialize_element(buffer, value.name);
        network::serialize_element(buffer, value.network_id);
        network::serialize_element(buffer, value.team);
        network::serialize_element(buffer, value.ready);
    }

    static void deserialize(std::string_view &buffer, LobbyMember &value)
    {
        network::deserialize_element(buffer, value.name);
        network::deserialize_element(buffer, value.network_id);
        network::deserialize_element(buffer, value.team);
        network::deserialize_element(buffer, value.ready);
    }

    std::string name{""};
    unsigned network_id{0};
    Team team{Neutral};
    bool ready{false};
};

class Lobby
{
public:
    Lobby() = default;

    Lobby(const std::string &id)
        : id{id}
    {
    }

    std::size_t getNumberOfPlayers()
    {
        return players.size();
    }

    std::vector<LobbyMember> getLobbyMember()
    {
        std::vector<LobbyMember> member;

        for (const auto &[_, mem] : players)
            member.push_back(mem);
        return member;
    }

    bool areAllMembersReady()
    {
        for(const auto& [_, member]: players)
            if(!member.ready)
                return false;

        return true;
    }

    void addPlayer(const LobbyMember &member)
    {
        players[member.network_id] = member;
    }

    std::map<unsigned, LobbyMember> players{};
    std::string id;
};
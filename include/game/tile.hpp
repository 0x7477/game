#pragma once

#include <game/movement.hpp>
#include <SFML/Graphics.hpp>
#include <map>
#include <helper/resource_loader.hpp>
#include <functional>
#include <cassert>
#include <memory>
#include <game/teams.hpp>
#include <game/unit.hpp>
#include <game/visualization_mode.hpp>
class Player;
class Map;
class Tile;

struct TileUnitInteraction
{
    virtual void interact(Tile &tile, Map &map) const {}
};

struct TilePlayerInteraction
{
    virtual bool interact(Tile &tile, Map &map) const {return false;}
};

struct NoTilePlayerInteraction : public TilePlayerInteraction
{
};

static NoTilePlayerInteraction default_tile_interaction;

class TileRoundBehaviour
{
    virtual void handleEndOfTurn();
    virtual void handleStartOfTurn();
};

class Tile
{
public:
    enum DisplayMode
    {
        Normal,
        Move,
        Attack
    };

    struct TileInfo
    {
        unsigned defense;
        MovementCosts movement_costs;
        TilePlayerInteraction &player_interaction = default_tile_interaction;
    };

    static std::string getClassName(const std::source_location &location = std::source_location::current())
    {
        const std::string function_name{location.function_name()};
        const auto start{function_name.find_last_of(':') + 1};
        const auto end{function_name.find_last_of(']')};

        if (end == std::string::npos)
            return function_name.substr(7, function_name.find(':', 8) - 7);

        return function_name.substr(start, end - start);
    }


    Tile(const TileInfo &info, const std::source_location &location = std::source_location::current())
        : id{getClassName(location)}, info{info}, sprite{"tiles/" + id + "/" + std::to_string(team)+  "/", "resources/images/"},
        movement_effect{"misc/movement_tile/", "resources/images/"},
        attack_effect{"misc/attack_tile/", "resources/images/"}
    {
        
    }


    void display(sf::RenderWindow &window, const Map &map, const TileIndex &index);
    void displayUnit(sf::RenderWindow &window, const Map &map, const TileIndex &index);

    void setDisplayMode(const DisplayMode &new_display_mode)
    {
        display_mode = new_display_mode;
    }

    bool interact(Tile &tile, Map &map)
    {
        return info.player_interaction.interact(tile, map);
    }


    template <typename T>
    static void registerClass(const unsigned &id)
    {
        library[id] = [=]() -> T
        { return T{}; };
    }

    template <typename T>
    static void registerClass(const unsigned &id, const Team &team)
    {
        library[id] = [team]() -> T
        { T t{};
          t.setTeam(team);
        return t; };
    }

    unsigned getDefense() const
    {
        return info.defense;
    }

    unsigned getMovementCost(const MovementType &type)
    {
        return info.movement_costs.getCosts(type);
    }

    std::string getId() const
    {
        return id;
    }

    void setTeam(const Team &new_team)
    {
        team = new_team;
        sprite = UI::GIF("tiles/" + id + "/" + std::to_string(team)+  "/", "resources/images/");
    }
    static Tile createTile(unsigned id)
    {
        assert(library.contains(id));
        return library[id]();
    }

    std::shared_ptr<Unit> unit{};

protected:
    std::string id;
    TileInfo info;
    DisplayMode display_mode{Normal};
    // TileUnitInteraction &unit_interaction;
    // TileRoundBehaviour &round_behaviour;


public:
    Team team{Neutral};
    UI::GIF sprite;
    UI::GIF movement_effect;
    UI::GIF attack_effect;
    // UI::GIF attack_effect;
    // const TilePlayerInteraction &player_interaction;

private:
    static inline std::map<unsigned, std::function<Tile()>> library;
};

template <typename T>
static bool init_tile(const unsigned &id)
{
    Tile::registerClass<T>(id);
    return true;
}

template <typename T>
static bool init_tile(const unsigned &id, const Team &team)
{
    Tile::registerClass<T>(id, team);
    return true;
}
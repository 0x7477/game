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
    virtual void interact(Map &, const TileIndex &) const {}
};

struct TileAttackInteraction
{
    virtual bool isAttackable(Map &, const TileIndex &, const Unit &) const { return false; }
    virtual void attack(Map &, const TileIndex &, const TileIndex &) const {}
};

struct TilePlayerInteraction
{
    virtual bool interact(Map &, const TileIndex &) const { return false; }
};

struct NoTilePlayerInteraction : public TilePlayerInteraction
{
};

struct ProduceUnit : public TilePlayerInteraction
{
    ProduceUnit(const std::vector<std::string> &factory_produced_units)
        : factory_produced_units{factory_produced_units} {}

    virtual bool interact(Map &map, const TileIndex &tile_index) const override;

    std::vector<std::string> factory_produced_units;
};

struct TileRoundBehaviour
{
    virtual void handleEndOfTurn(Map& , const TileIndex &){};
    virtual void handleStartOfTurn(Map&, const TileIndex &){};
};
static NoTilePlayerInteraction default_tile_interaction;
static TileAttackInteraction default_tile_attack_interaction;
static TileRoundBehaviour default_tile_round_behaviour;

struct RepairUnits : TileRoundBehaviour
{
    virtual void handleStartOfTurn(Map& map, const TileIndex &tile_index);
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

    enum Direction
    {
        URDL,
        V,
        H,
        U,
        UR,
        R,
        RD,
        D,
        DL,
        L,
        LU,
        URD,
        RDL,
        DLU,
        LUR
    };

    struct TileInfo
    {
        unsigned defense;
        MovementCosts movement_costs;
        TilePlayerInteraction &player_interaction = default_tile_interaction;
        TileAttackInteraction &tile_attack_interaction = default_tile_attack_interaction;
        TileRoundBehaviour& tile_round_behaviour = default_tile_round_behaviour;
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
        : id{getClassName(location)}, info{info}, sprite{"tiles/" + id + "/" + std::to_string(team) + "/", "resources/images/"},
          movement_effect{"misc/movement/", "resources/images/"},
          attack_effect{"misc/attack_tile/", "resources/images/"}
    {
    }

    void display(sf::RenderWindow &window, const Map &map, const TileIndex &index);
    void displayUnit(sf::RenderWindow &window, const Map &map, const TileIndex &index);

    void setDisplayMode(const DisplayMode &new_display_mode)
    {
        display_mode = new_display_mode;
    }

    bool interact(Map &map, const TileIndex &tile)
    {
        return info.player_interaction.interact(map, tile);
    }

    void attack(Map &map, const TileIndex &unit, const TileIndex &tile)
    {
        return info.tile_attack_interaction.attack(map, unit, tile);
    }

    bool isAttackable(Map &map, const TileIndex &tile, const Unit &unit)
    {
        return info.tile_attack_interaction.isAttackable(map, tile, unit);
    }

    template <typename T>
    static void registerClass(const unsigned &id)
    {
        library[id] = [=]()
        { return std::make_unique<T>(); };
    }

    template <typename T>
    static void registerClass(const unsigned &id, const Team &team)
    {
        library[id] = [team]() -> std::unique_ptr<T>
        { std::unique_ptr<T> t{std::make_unique<T>()};
          t->setTeam(team);
        return t; };
    }

    template <typename T>
    static void registerClass(const unsigned &id, const Direction &direction)
    {
        library[id] = [direction]() -> std::unique_ptr<T>
        { std::unique_ptr<T> t{std::make_unique<T>()};
          t->setDirection(direction);
        return t; };
    }

    void setDirection(const Direction &direction_)
    {
        direction = direction_;
        sprite = UI::GIF("tiles/" + id + "/" + std::to_string(direction) + "/", "resources/images/");
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

    
    void startRound(Map& map, const TileIndex& index)
    {
        info.tile_round_behaviour.handleStartOfTurn(map, index);
    }


    void setTeam(const Team &new_team)
    {
        team = new_team;
        sprite = UI::GIF("tiles/" + id + "/" + std::to_string(team) + "/", "resources/images/");
    }
    static std::unique_ptr<Tile> createTile(unsigned id)
    {
        if (!library.contains(id))
            std::cout << "missing id " << id << "\n";
        assert(library.contains(id));
        return library[id]();
    }

    std::shared_ptr<Unit> unit{};

protected:
    std::string id;
    DisplayMode display_mode{Normal};
    // TileUnitInteraction &unit_interaction;
    // TileRoundBehaviour &round_behaviour;

public:
    TileInfo info;
    Team team{Neutral};
    UI::GIF sprite;
    UI::GIF movement_effect;
    UI::GIF attack_effect;
    Direction direction{URDL};
    // UI::GIF attack_effect;
    // const TilePlayerInteraction &player_interaction;

private:
    static inline std::map<unsigned, std::function<std::unique_ptr<Tile>()>> library;
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

template <typename T>
static bool init_tile(const unsigned &id, const Tile::Direction &direction)
{
    Tile::registerClass<T>(id, direction);
    return true;
}
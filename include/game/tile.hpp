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
    ProduceUnit(const std::vector<std::string> &factory_produced_units);

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

    static std::string getClassName(const std::source_location &location = std::source_location::current());

    Tile(const TileInfo &info, const std::source_location &location = std::source_location::current());

    void display(sf::RenderWindow &window, const Map &map, const TileIndex &index);
    void displayUnit(sf::RenderWindow &window, const Map &map, const TileIndex &index);

    void setDisplayMode(const DisplayMode &new_display_mode);

    bool interact(Map &map, const TileIndex &tile);

    void attack(Map &map, const TileIndex &unit, const TileIndex &tile);

    bool isAttackable(Map &map, const TileIndex &tile, const Unit &unit);

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

    void setDirection(const Direction &direction_);

    unsigned getDefense() const;

    unsigned getMovementCost(const MovementType &type);

    std::string getId() const;

    
    void startRound(Map& map, const TileIndex& index);


    void setTeam(const Team &new_team);
    static std::unique_ptr<Tile> createTile(unsigned id);

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
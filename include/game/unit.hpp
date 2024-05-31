#pragma once

#include <game/movement.hpp>
#include <SFML/Graphics.hpp>
#include <helper/resource_loader.hpp>
#include <functional>
#include <optional>
#include <cassert>
#include <source_location>
#include <game/tile_index.hpp>
#include <game/movement_manager.hpp>
#include <optional>
#include <game/teams.hpp>
#include <ui/gif.hpp>
#include <array>
#include <game/unit_names.hpp>

class Tile;
class Map;

class Unit
{

public:
    struct Stats
    {
        Stats(const unsigned &movement_speed, const MovementType &movement_type, const unsigned &ammo, const unsigned &attack_range_min = 1, const unsigned &attack_range_max = 1);
        
        unsigned movement_speed{0};
        MovementType movement_type;
        unsigned attack_range_min{0}, attack_range_max{0};
        unsigned max_ammo{0};
    };

    struct Status
    {
        bool finished : 1 {false};

        bool canAct() const;
    };

    struct Action
    {
        std::string name;
        std::function<void(const unsigned& index )> execute;
    };

    typedef std::function<void(Map &map, const TileIndex &me, const TileIndex &new_position, const TileIndex &target, const unsigned& index)> UnitActionFunction;

    enum ActionId : uint8_t
    {
        Wait,
        Attack,
        Unload,
        Load,
        Capture,
        Fire,
        Join,
        ActionIdCount
    };

    void refuel();

    Unit(const Team &team, const Stats &stats, const std::source_location &location = std::source_location::current());

    bool receivesTerrainBonus() const {return stats.movement_type != Air;}
    void displayPath(sf::RenderWindow &window, Map &map);
    void display(sf::RenderWindow &window, Map &map, const TileIndex &index);
    virtual void displayAdditions(sf::RenderWindow &window, Map &map, const TileIndex &index);

    std::optional<Action> getJoinAction(Map &map, const TileIndex &me, const TileIndex &target);

    virtual void onMoved() {};

    virtual void startRound(const Map &, const TileIndex &);

    template <typename T>
    static void registerClass()
    {
        const auto id = getClassName();
        library[id] = [=](const Team &team) -> std::shared_ptr<Unit>
        { return std::make_shared<T>(team); };
    }

    static std::shared_ptr<Unit> createUnit(const std::string &id, const Team &team);

    static std::string getClassName(const std::source_location &location = std::source_location::current());

    // actions
    void executeAction(const ActionId &action, Map &map, const TileIndex &me, const TileIndex &new_position, const TileIndex &target, const unsigned& index);
    void executeWaitAction(Map &map, const TileIndex &me, const TileIndex &new_position);
    void executeLoadAction(Map &map, const TileIndex &me, const TileIndex &new_position);
    void executeAttackAction(Map &map, const TileIndex &me, const TileIndex &new_position, const TileIndex &target);
    void executeJoinAction(Map &map, const TileIndex &me, const TileIndex &new_position);

    bool select(Map &map, const TileIndex &index);
    void act(Map &map, const TileIndex &me, const TileIndex &target);
    void move(Map &map, const TileIndex &from, const TileIndex &to);
    void displayMovementTiles(Map &map, const TileIndex &index);

    unsigned getCosts();
    void heal(const unsigned &amount);

    MovementType getMovementType() const;
    unsigned getMovementSpeed() const;

    bool testIfCanBeJoinedWithUnit(const Unit &unit);

    virtual bool allowUnitInteraction(const Unit &unit);

    virtual std::vector<Action> getUnitInteractionOption(Map &map, const TileIndex &me, const TileIndex &target);

    virtual void executeUnitInteraction(Map &map, const TileIndex &from, const TileIndex &to);

    virtual std::vector<Action> handlePossibleActions(Map &, const TileIndex &, const TileIndex &);

    Team getTeam() const;

    void setTeam(const Team &team_) { team = team_; }
    void setFinished(const bool &finished = true) { status.finished = finished; }

    unsigned getUnitCount() const;
    unsigned getHealth() const;

    void updateCursor(Map &map, const TileIndex &cursor);
    std::string id;
    constexpr static int max_health{100};
    int health{max_health};
    static inline std::map<std::string, std::function<std::shared_ptr<Unit>(const Team &)>> library;

    void endTurn(Map &map, const bool &finished = true);

    bool isRangedUnit() const;

public:
    Stats stats;
protected:
    Team team{Team::Red};

    std::array<UnitActionFunction, ActionIdCount> actions{nullptr};

public:
    Status status;
    MovementManager movement_manager;
    unsigned ammo;
    static std::map<std::string, unsigned> unit_costs;

protected:
    sf::Sprite health_text;
};

template <typename T>
static bool init_unit()
{
    Unit::registerClass<T>();
    return true;
}
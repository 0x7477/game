#pragma once

#include <game/movement.hpp>
#include <SFML/Graphics.hpp>
#include <helper/resource_loader.hpp>
#include <functional>
#include <cassert>
#include <game/movement.hpp>
#include <source_location>
#include <game/tile_index.hpp>
#include <game/movement_manager.hpp>
#include <optional>
#include <game/teams.hpp>
#include <ui/gif.hpp>
#include <array>

class Tile;
class Map;

class Unit
{

public:
    struct Stats
    {
        unsigned movement_speed;
        MovementType movement_type;
    };

    struct Status
    {
        bool finished : 1 {false};

        bool canAct() const
        {
            return !finished;
        }
    };

    struct Action
    {
        std::string name;
        std::function<void()> execute;
    };

    typedef std::function<void(Map& map, const TileIndex &me, const TileIndex &new_position, const TileIndex &target)> UnitActionFunction;

    enum ActionId : uint8_t
    {
        Wait,
        Attack,
        Unload,
        Load,
        Capture,
        Fire,
        ActionIdCount
    };

    Unit(const Team &team, const Stats &stats, const std::source_location &location = std::source_location::current());

    void displayPath(sf::RenderWindow &window, Map &map);
    void display(sf::RenderWindow &window, const Map &map, const TileIndex &index);


    virtual void onMoved(){};

    virtual void startRound(const Map &map, const TileIndex &index) {
        setFinished(false);
    };

    template <typename T>
    static void registerClass()
    {
        const auto id = getClassName();
        library[id] = [=](const Team &team) -> std::shared_ptr<Unit>
        { return std::make_shared<T>(team); };
    }

    static std::shared_ptr<Unit> createUnit(const std::string &id, const Team &team);

    static std::string getClassName(const std::source_location &location = std::source_location::current())
    {
        const std::string function_name{location.function_name()};
        const auto start{function_name.find_last_of(':') + 1};
        const auto end{function_name.find_last_of(']')};

        if (end == std::string::npos)
            return function_name.substr(7, function_name.find(':', 8) - 7);

        return function_name.substr(start, end - start);
    }

    

    //actions
    void executeAction(const ActionId& action, Map &map, const TileIndex &me, const TileIndex &new_position, const TileIndex &target);
    void executeWaitAction(Map &map, const TileIndex &me, const TileIndex &new_position, const TileIndex &target = {});
    void executeLoadAction(Map &map, const TileIndex &me, const TileIndex &new_position, const TileIndex &target = {});
    void executeUnloadAction(Map &map, const TileIndex &me, const TileIndex &new_position, const TileIndex &target = {});
    void executeAttackAction(Map &map, const TileIndex &me, const TileIndex &new_position, const TileIndex &target);


    bool select(Map &map, const TileIndex &index);
    void act(Map &map, const TileIndex &me, const TileIndex &target);
    void move(Map &map, const TileIndex &from, const TileIndex &to);
    void displayMovementTiles(Map &map, const TileIndex &index);

    MovementType getMovementType() const { return stats.movement_type; }
    unsigned getMovementSpeed() const { return stats.movement_speed; }

    virtual bool allowUnitInteraction(const Unit &unit)
    {
        return false;
    }

    virtual std::vector<Action> getUnitInteractionOption(Map &map, const TileIndex &unit, const TileIndex &me) { return {}; }; // TODO ADD JOIN HERE

    virtual void executeUnitInteraction(Map &map, const TileIndex &unit, const TileIndex &me) {}

    virtual std::vector<Action> handlePossibleActions(Map &map, const TileIndex &me, const TileIndex &target) { return {}; }

    Team getTeam() const { return team; }

    void setTeam(const Team &team_) { team = team_; }
    void setFinished(const bool &finished = true) { status.finished = finished; }

    unsigned getUnitCount() const;

    void updateCursor(Map &map, const TileIndex &cursor);
    std::string id;
    UI::GIF sprite;
    constexpr static int max_health{100};
    int health{max_health};
    unsigned ammo{0};
    static inline std::map<std::string, std::function<std::shared_ptr<Unit>(const Team &)>> library;

    void endTurn(Map &map, const bool& finished = true);

protected:
    Stats stats;
    Status status;
    Team team{Team::Red};

    std::array<UnitActionFunction, ActionIdCount> actions{nullptr};

public:
    MovementManager movement_manager;

protected:
    sf::Text health_text;
};

template <typename T>
static bool init_unit()
{
    Unit::registerClass<T>();
    return true;
}
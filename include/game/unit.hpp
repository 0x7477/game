#pragma once

#include <game/movement.hpp>
#include <SFML/Graphics.hpp>
#include <helper/resource_loader.hpp>
#include <functional>
#include <cassert>
#include <game/movement.hpp>
#include <source_location>
#include <game/tile_index.hpp>
#include <game/movement_animation.hpp>
#include <optional>
#include <game/teams.hpp>
#include <ui/gif.hpp>

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
    
    Unit(const Team& team, const Stats &stats, const std::source_location &location = std::source_location::current());

    void display(sf::RenderWindow &window, const Map &map, const TileIndex &index);

    virtual void startRound(const Map &map, const TileIndex &index) {};

    template <typename T>
    static void registerClass()
    {
        const auto id = getClassName();
        library[id] = [=](const Team& team) -> std::shared_ptr<Unit>
        { return std::make_shared<T>(team); };
    }

    static std::shared_ptr<Unit> createUnit(const std::string &id, const Team& team);

    static std::string getClassName(const std::source_location &location = std::source_location::current())
    {
        const std::string function_name{location.function_name()};
        const auto start{function_name.find_last_of(':') + 1};
        const auto end{function_name.find_last_of(']')};

        if (end == std::string::npos)
            return function_name.substr(7, function_name.find(':', 8) - 7);

        return function_name.substr(start, end - start);
    }

    bool select(Map &map, const TileIndex &index);
    void act(Map &map, const TileIndex &me, const TileIndex &target);

    void displayMovementTiles(Map &map, const TileIndex &index);

    void attack(Map &map, const TileIndex &me, const TileIndex &target);


    MovementType getMovementType() const { return stats.movement_type; }
    unsigned getMovementSpeed() const { return stats.movement_speed; }

    virtual bool allowUnitInteraction(const Unit &unit)
    {
        return false;
    }

    virtual std::vector<Action> getUnitInteractionOption(Map &map, const TileIndex &unit, const TileIndex &me) {return {};}; //TODO ADD JOIN HERE


    virtual void executeUnitInteraction(Map &map, const TileIndex &unit,const TileIndex &me) {}

    virtual std::vector<Action> handlePossibleActions(Map &map, const TileIndex &me,const TileIndex &target){return {};}

    Team getTeam() const { return team; }

    void setTeam(const Team &team_) { team = team_; }
    void setFinished(const bool& finished = true) {status.finished = finished; }

    unsigned getUnitCount() const;

    std::string id;
    UI::GIF sprite;
    constexpr static int max_health{100};
    int health{max_health};
    unsigned ammo{0};
    static inline std::map<std::string, std::function<std::shared_ptr<Unit>(const Team&)>> library;

    void endTurn(Map& map);
protected:

    
    Stats stats;
    Status status;
    Team team{Team::Red};

    std::optional<MovementAnimation> movement_animation;
    sf::Text health_text;
};

template <typename T>
static bool init_unit()
{
    Unit::registerClass<T>();
    return true;
}
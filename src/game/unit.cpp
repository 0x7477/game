#include <game/unit.hpp>

#include <game/movement.hpp>
#include <SFML/Graphics.hpp>
#include <helper/resource_loader.hpp>
#include <functional>
#include <cassert>
#include <game/movement.hpp>
#include <game/map.hpp>
#include <game/tile_selector.hpp>
#include <game/attack_simulator.hpp>
#include <scenes/battle.hpp>
#include <game/tile.hpp>

Unit::Unit(const Team &team, const Stats &stats, const std::source_location &location)
    : id{getClassName(location)},
      stats{stats}, team{team}, movement_manager{*this}, ammo{stats.max_ammo}, health_text{image_resources.get("numbers/1.png")}
{
    actions[Wait] = [this](Map &map, const TileIndex &me, const TileIndex &new_position, const TileIndex &, const unsigned &)
    { this->executeWaitAction(map, me, new_position); };
    actions[Attack] = [this](Map &map, const TileIndex &me, const TileIndex &new_position, const TileIndex &target, const unsigned &)
    { this->executeAttackAction(map, me, new_position, target); };
    actions[Load] = [this](Map &map, const TileIndex &me, const TileIndex &new_position, const TileIndex &, const unsigned &)
    { this->executeLoadAction(map, me, new_position); };
    actions[Join] = [this](Map &map, const TileIndex &me, const TileIndex &new_position, const TileIndex &, const unsigned &)
    { this->executeJoinAction(map, me, new_position); };

    health_text.setOrigin(health_text.getTexture()->getSize().x, health_text.getTexture()->getSize().y);
}

void Unit::displayPath(sf::RenderWindow &window, Map &map)
{
    movement_manager.displayPath(window, map);
}

void Unit::refuel()
{
    ammo = stats.max_ammo;
}

void Unit::updateCursor(Map &map, const TileIndex &cursor)
{
    movement_manager.updatePath(map, cursor);
}
void Unit::displayAdditions(sf::RenderWindow &,  Map &, const TileIndex &)
{

}


void Unit::display(sf::RenderWindow &window, Map &map, const TileIndex &index)
{
    movement_manager.draw(window, map, index);

    if (movement_manager.isMoving())
        return;

    displayAdditions(window, map, index);
    
    if (getUnitCount() == 10)
        return;

    const auto offset = map.scale * 3.0;
    const auto [x, y] = movement_manager.getCurrentPosition(map, index);

    health_text.setPosition(x + 16 * map.scale, y + 16 * map.scale - offset);
    const auto size = health_text.getTexture()->getSize();
    health_text.setScale(8 * map.scale / size.x, 8 * map.scale / size.y);
    health_text.setTexture(image_resources.get("numbers/" + std::to_string(getUnitCount()) + ".png"));
    window.draw(health_text);

}

unsigned Unit::getCosts()
{
    return unit_costs[id];
}

void Unit::heal(Map& map, const TileIndex& position, const unsigned &amount)
{
    map.animations.emplace_back(Animation{UI::GIF("misc/heal/", "resources/images/"), 1, position, 0,0});
    health += amount;
}

std::shared_ptr<Unit> Unit::createUnit(const std::string &id, const Team &team)
{
    if (!library.contains(id))
        std::cout << id << "\n";
    assert(library.contains(id));
    return library[id](team);
}

Team Unit::getTeam() const
{
    return team;
}

Unit::Stats::Stats(const unsigned &movement_speed, const MovementType &movement_type, const unsigned &ammo, const unsigned &attack_range_min, const unsigned &attack_range_max)
    : movement_speed{movement_speed}, movement_type{movement_type}, attack_range_min{attack_range_min}, attack_range_max{attack_range_max}, max_ammo{ammo}
{
}

bool Unit::Status::canAct() const
{
    return !finished;
}

void Unit::startRound(const Map &, const TileIndex &) {
};
std::string Unit::getClassName(const std::source_location &location)
{
    const std::string function_name{location.function_name()};
    const auto start{function_name.find_last_of(':') + 1};
    const auto end{function_name.find_last_of(']')};

    if (end == std::string::npos)
        return function_name.substr(7, function_name.find(':', 8) - 7);

    return function_name.substr(start, end - start);
}

MovementType Unit::getMovementType() const
{
    return stats.movement_type;
}
unsigned Unit::getMovementSpeed() const
{
    return stats.movement_speed;
}

bool Unit::testIfCanBeJoinedWithUnit(const Unit &unit)
{
    return getUnitCount() < 10 && id == unit.id;
}

bool Unit::allowUnitInteraction(const Unit &unit)
{
    return testIfCanBeJoinedWithUnit(unit);
}

std::vector<Unit::Action> Unit::getUnitInteractionOption(Map &map, const TileIndex &me, const TileIndex &target)
{
    const auto join_action_option = getJoinAction(map, me, target);
    if (join_action_option)
        return {*join_action_option};
    return {};
}

void Unit::executeUnitInteraction(Map &map, const TileIndex &from, const TileIndex &to)
{
    executeJoinAction(map, from, to);
}

std::vector<Unit::Action> Unit::handlePossibleActions(Map &, const TileIndex &, const TileIndex &)
{
    return {};
}

bool Unit::isRangedUnit() const
{
    return stats.attack_range_min > 1;
}

void Unit::displayMovementTiles(Map &map, const TileIndex &index)
{
    const auto possible_movement_tiles = MovementSelector::getTiles(map, index, *this, true);

    for (const auto &tile_index : possible_movement_tiles)
        map[tile_index].setDisplayMode(Move);
}

bool Unit::select(Map &map, const TileIndex &index)
{
    if (!status.canAct())
        return false;
    map.selected_unit = index;
    movement_manager.init(index);
    displayMovementTiles(map, index);
    return true;
}

void Unit::move(Map &map, const TileIndex &from, const TileIndex &to)
{
    if (from == to)
        return;
    auto &target_tile = map[to];

    if (target_tile.unit)
    {
        std::cout << "executeUnitInteraction\n";
        target_tile.unit->executeUnitInteraction(map, from, to);
    }
    else
    {
        target_tile.unit = map[from].unit;
        map[from].unit = nullptr;
    }
    onMoved();
}

void Unit::executeAction(const ActionId &action, Map &map, const TileIndex &me, const TileIndex &new_position, const TileIndex &target, const unsigned &index)
{
    assert(actions[action] != nullptr);
    actions[action](map, me, new_position, target, index);
}

void Unit::executeWaitAction(Map &map, const TileIndex &me, const TileIndex &new_position)
{
    move(map, me, new_position);
    endTurn(map);
}

void Unit::executeLoadAction(Map &map, const TileIndex &me, const TileIndex &new_position)
{
    move(map, me, new_position);
    endTurn(map, false);
}

void Unit::executeAttackAction(Map &map, const TileIndex &me, const TileIndex &new_position, const TileIndex &target)
{
    move(map, me, new_position);


    map.animations.emplace_back(Animation{UI::GIF("misc/attack/", "resources/images/"), 0.81,target, 1,1});

    if (map[target].unit)
    {
        const auto result = AttackSimulator::attack(map, new_position, target);
        if (result.attacker_died)
            map.killUnit(new_position);

        if (result.defender_died)
            map.killUnit(target);
    }
    else
    {
        map[target].attack(map, new_position, target);
    }

    endTurn(map);
    map.clearTileEffects();
}

void Unit::act(Map &map, const TileIndex &me, const TileIndex &target)
{
    if (!status.canAct())
        return;

    const auto possible_movement_tiles = MovementSelector::getTiles(map, me, *this);
    bool is_move_valid = std::find(possible_movement_tiles.begin(), possible_movement_tiles.end(), target) != possible_movement_tiles.end();

    if (!is_move_valid)
        return;

    map.clearTileEffects();

    auto on_movement_finished = [&map, target, me, this]()
    {
        std::map<std::string, std::function<void(const unsigned &index)>> actions{};
        std::vector<std::string> options{}; // need to check if wait or join

        const auto target_tile = map[target];
        const auto has_target_unit = target_tile.unit;

        if (!has_target_unit || target == me)
        {
            const auto possible_attack_tiles = AttackSelector::getTiles(map, target, *this, stats.attack_range_min, stats.attack_range_max);
            if (possible_attack_tiles.size() > 0 && (!isRangedUnit() || (isRangedUnit() && target == me)))
            {
                options.push_back("Attack");
                actions["Attack"] = [=, this, &map](const unsigned &)
                {
                    map.setMovementTileMode(possible_attack_tiles, TileDisplayMode::Attack);
                    map.mode = SelectTarget;
                    map.selectable_targets = possible_attack_tiles;

                    map.select_function = [this, &map, target, me](const TileIndex &unit_to_attack)
                    {
                        map.game.sendAction(Attack, movement_manager.getPath(), unit_to_attack);
                        executeAttackAction(map, me, target, unit_to_attack);
                    };
                };
            }
        }

        const auto unit_actions = handlePossibleActions(map, me, target);
        for (const auto &unit_action : unit_actions)
        {
            options.push_back(unit_action.name);
            actions[unit_action.name] = unit_action.execute;
        }

        if (has_target_unit)
        {
            const auto unit_to_unit_actions = target_tile.unit->getUnitInteractionOption(map, me, target);

            for (const auto &unit_action : unit_to_unit_actions)
            {
                options.push_back(unit_action.name);
                actions[unit_action.name] = unit_action.execute;
            }
        }

        if (!has_target_unit || target == me)
        {
            options.push_back("Wait");

            actions["Wait"] = [=, this, &map](const unsigned &)
            {
                executeWaitAction(map, me, target);
                map.game.sendAction(Wait, movement_manager.getPath(), target);
            };
        }
        auto execute_action = [this, options, actions](const std::size_t &index) mutable
        {
            auto option = options[index];
            auto func = actions[option];
            func(index);
        };
        auto cancel_action = [&, me]()
        {
            movement_manager.stop();
            displayMovementTiles(map, me);
            map.mode = View;
        };

        map.action_menu.setOptions(options, execute_action, cancel_action);
    };

    map.action_menu.clearOptions();
    map.mode = SelectAction;
    movement_manager.startAnimation(on_movement_finished);
}

void Unit::executeJoinAction(Map &map, const TileIndex &from, const TileIndex &to)
{
    std::cout << "executeJoinAction basic\n";
    const int health = map[from].unit->getUnitCount();
    const int missing_health = 10 - map[to].unit->getUnitCount();
    const int healing = std::min(health, missing_health);
    const int money_compensation = (health - healing) * map[from].unit->getCosts() / 10;
    map.game.players[map[from].unit->getTeam()].money += money_compensation;
    map[to].unit->heal(map, to, healing * 10);
    map[to].unit->setFinished();
    map[from].unit = nullptr;
    endTurn(map);
}

std::optional<Unit::Action> Unit::getJoinAction(Map &map, const TileIndex &me, const TileIndex &target)
{
    if (me == target)
        return {};
    if (!map[target].unit)
        return {};
    if (map[target].unit->getUnitCount() == 10)
        return {};
    if (map[me].unit->id != map[target].unit->id )
        return {};

    return Action{.name = "Join", .execute = [&map, me, target, this](const unsigned &)
                                  {
                                      std::cout << "movement_manager.getPath()" << "\n";
                                      std::cout << movement_manager.getPath().size() << "\n";
                                      map.game.sendAction(Join, map[me].unit->movement_manager.getPath(), target);
                                      executeJoinAction(map, me, target);
                                  }};
}

unsigned Unit::getUnitCount() const
{
    if (health < 0)
        return 0;
    return (health + 9) / 10;
}

unsigned Unit::getHealth() const
{
    if (health < 0)
        return 0;
    return health;
}

void Unit::endTurn(Map &map, const bool &set_finished)
{
    movement_manager.stop();
    map.selected_unit = {};
    map.mode = View;
    if (set_finished)
        setFinished();
}
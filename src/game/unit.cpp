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

Unit::Unit(const Team &team, const Stats &stats, const std::source_location &location)
    : id{getClassName(location)},
      stats{stats}, team{team}, movement_manager{*this}, health_text{image_resources.get("numbers/1.png")}
{
    actions[Wait] = [this](Map &map, const TileIndex &me, const TileIndex &new_position, const TileIndex &target)
    { this->executeWaitAction(map, me, new_position, target); };
    actions[Attack] = [this](Map &map, const TileIndex &me, const TileIndex &new_position, const TileIndex &target)
    { this->executeAttackAction(map, me, new_position, target); };
    actions[Load] = [this](Map &map, const TileIndex &me, const TileIndex &new_position, const TileIndex &target)
    { this->executeLoadAction(map, me, new_position, target); };
    actions[Join] = [this](Map &map, const TileIndex &me, const TileIndex &new_position, const TileIndex &target)
    { this->executeJoinAction(map, me, new_position, target); };

    health_text.setOrigin(health_text.getTexture()->getSize().x, health_text.getTexture()->getSize().y);
}

void Unit::displayPath(sf::RenderWindow &window, Map &map)
{
    movement_manager.displayPath(window, map);
}

void Unit::updateCursor(Map &map, const TileIndex &cursor)
{
    movement_manager.updatePath(map, cursor);
}

void Unit::display(sf::RenderWindow &window, const Map &map, const TileIndex &index)
{
    movement_manager.draw(window, map, index);

    if (movement_manager.isMoving())
        return;
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


void Unit::heal(const unsigned &amount)
{
    health += amount;
}

std::shared_ptr<Unit> Unit::createUnit(const std::string &id, const Team &team)
{
    assert(library.contains(id));
    return library[id](team);
}

void Unit::displayMovementTiles(Map &map, const TileIndex &index)
{
    const auto possible_movement_tiles = MovementSelector::getTiles(map, index, *this);

    for (const auto &tile_index : possible_movement_tiles)
        map[tile_index].setDisplayMode(Tile::Move);
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

void Unit::executeAction(const ActionId &action, Map &map, const TileIndex &me, const TileIndex &new_position, const TileIndex &target)
{
    assert(actions[action] != nullptr);
    actions[action](map, me, new_position, target);
}

void Unit::executeWaitAction(Map &map, const TileIndex &me, const TileIndex &new_position, const TileIndex &)
{
    move(map, me, new_position);
    endTurn(map);
}

void Unit::executeLoadAction(Map &map, const TileIndex &me, const TileIndex &new_position, const TileIndex &)
{
    move(map, me, new_position);
    endTurn(map, false);
}

void Unit::executeAttackAction(Map &map, const TileIndex &me, const TileIndex &new_position, const TileIndex &target)
{
    move(map, me, new_position);

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
        std::map<std::string, std::function<void()>> actions{};
        std::vector<std::string> options{}; // need to check if wait or join

        const auto target_tile = map[target];
        const auto has_target_unit = target_tile.unit;

        if (!has_target_unit || target == me)
        {
            const auto possible_attack_tiles = AttackSelector::getTiles(map, target, *this, 1, 1);
            if (possible_attack_tiles.size() > 0)
            {
                options.push_back("Attack");
                actions["Attack"] = [=, this, &map]()
                {
                    map.setMovementTileMode(possible_attack_tiles, Tile::DisplayMode::Attack);
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

            actions["Wait"] = [=, this, &map]()
            {
                executeWaitAction(map, me, target, target);
                map.game.sendAction(Wait, movement_manager.getPath(), target);
            };
        }
        auto execute_action = [this, options, actions](const std::size_t &index) mutable
        {
            auto option = options[index];
            auto func = actions[option];
            func();
        };
        auto cancel_action = [&, me]()
        {
            movement_manager.stop();
            displayMovementTiles(map, me);
            map.mode = View;
        };

        map.action_menu.setOptions(options, execute_action, cancel_action);
    };

    movement_manager.startAnimation(on_movement_finished);
    map.mode = SelectAction;
    map.action_menu.clearOptions();
}

void Unit::executeJoinAction(Map &map, const TileIndex &from, const TileIndex &to, const TileIndex &)
{
    const auto health = 10 * map[from].unit->getUnitCount();
    map[to].unit->heal(health);
    map[to].unit->setFinished();
    map[from].unit = nullptr;
    endTurn(map);
}


std::optional<Unit::Action> Unit::getJoinAction(Map &map,const TileIndex &me, const TileIndex &target)
{
    if(me == target)
        return{};
    if(!map[target].unit)
        return{};
    if(map[target].unit->getUnitCount() == 10)
        return{};

    return Action{.name = "Join", .execute = [&map, me, target, this]()
    {
        std::cout << "movement_manager.getPath()" << "\n";
        std::cout  << movement_manager.getPath().size() << "\n";
        map.game.sendAction(Join, map[me].unit->movement_manager.getPath(), target);
        executeJoinAction(map, me, target, target);
    }};
}

unsigned Unit::getUnitCount() const
{
    if (health < 0)
        return 0;
    return (health + 9) / 10;
}

void Unit::endTurn(Map &map, const bool &set_finished)
{
    movement_manager.stop();
    map.selected_unit = {};
    map.mode = View;
    if (set_finished)
        setFinished();
}
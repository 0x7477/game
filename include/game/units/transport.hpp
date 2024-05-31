#include <game/unit.hpp>
#include <game/map.hpp>
#include <game/game.hpp>
#include <game/tile_selector.hpp>
namespace Units
{

    template <unsigned capacity>
    class Transport : public Unit
    {

    public:
        Transport(const Team &team, const Stats &stats, const std::vector<std::string> &loadable_units, const std::source_location &location = std::source_location::current())
            : Unit{team, stats, location}, loadable_units{loadable_units}, star{image_resources.get("misc/star.png")}
        {
            actions[Unload] = [this](Map &map, const TileIndex &me, const TileIndex &new_position, const TileIndex &target, const unsigned &index)
            { this->executeUnloadAction(map, me, new_position, target, index); };
        }

        bool isUnitLoadable(const Unit &unit)
        {
            for (const auto &candidate : loadable_units)
                if (candidate == unit.id)
                    return true;
            return false;
        }

        virtual bool allowUnitInteraction(const Unit &unit) override
        {
            if (loaded_count == 0 && testIfCanBeJoinedWithUnit(unit)) // join
                return true;

            if (unit.id == id)
            {
                Transport<capacity> *other = (Transport<capacity> *)&unit;

                std::cout << loaded_count << " " << other->loaded_count << capacity << "\n";
                if (loaded_count + other->loaded_count > capacity) // cant load any further
                    return false;
            }

            return isUnitLoadable(unit); // can load if id matches
        }

        virtual void executeUnitInteraction(Map &map, const TileIndex &unit, const TileIndex &me) override
        {
            if (map[unit].unit->id == id)
            {
                std::cout << "Join\n";
                Transport<capacity> *other = (Transport<capacity> *)map[unit].unit.get();

                std::cout << "other->loaded_count "<<other->loaded_count<<" \n";
                std::cout << "loaded_count "<<loaded_count<<" \n";

                if (other->loaded_count > 0)
                {
                    loaded_units = other->loaded_units;
                    loaded_count = other->loaded_count;
                }
                else
                {
                    other->loaded_units = loaded_units;
                    other->loaded_count = loaded_count;
                }

                executeJoinAction(map, unit, me);
                return;
            }

            // load the unit
            loaded_units[loaded_count] = map[unit].unit;
            loaded_count++;
            map[unit].unit = nullptr;
        }

        void executeUnloadAction(Map &map, const TileIndex &me, const TileIndex &new_position, const TileIndex &target, const unsigned &index)
        {
            move(map, me, new_position);
            loaded_units[index]->setFinished();
            loaded_units[index]->movement_manager.init(new_position);
            loaded_units[index]->movement_manager.updatePath(map, target);

            map[target].unit = loaded_units[index];
            map[target].unit->movement_manager.startAnimation([&map, target]()
                                                              { map[target].unit->movement_manager.stop(); });
            for (unsigned i{index}; i < capacity - 1; i++)
                loaded_units[i] = loaded_units[i + 1];

            loaded_units[capacity - 1] = nullptr;
            loaded_count--;

            if (loaded_count == 0)
                endTurn(map);
        }

        void unloadUnit(Map &map, const TileIndex &me, const TileIndex &target, const unsigned &index)
        {
            const auto possible_unload_tiles = UnloadSelector::getTiles(map, target, *loaded_units[index]);
            map.setMovementTileMode(possible_unload_tiles, Tile::DisplayMode::Move);
            map.mode = SelectTarget;
            map.selectable_targets = possible_unload_tiles;

            map.select_function = [this, &map, me, target, index](const TileIndex &unload_tile)
            {
                map.game.sendAction(Unload, movement_manager.getPath(), unload_tile);
                map.clearTileEffects();
                executeUnloadAction(map, me, target, unload_tile, index);

                if (loaded_count == 0)
                    return;

                movement_manager.init(target);

                std::map<std::string, std::function<void(const unsigned &index)>> actions{};
                std::vector<std::string> options{}; // need to check if wait or join

                const auto unit_actions = handlePossibleActions(map, target, target);
                for (const auto &unit_action : unit_actions)
                {
                    options.push_back(unit_action.name);
                    actions[unit_action.name] = unit_action.execute;
                }

                options.push_back("Wait");

                actions["Wait"] = [=, this, &map](const unsigned &)
                {
                    executeWaitAction(map, me, target);
                    map.game.sendAction(Wait, movement_manager.getPath(), target);
                };

                auto execute_action = [this, options, actions](const std::size_t &index) mutable
                {
                    auto option = options[index];
                    auto func = actions[option];
                    func(index);
                };

                map.action_menu.setOptions(options, execute_action, {});
                map.mode = SelectAction;
            };
        }

        virtual std::vector<Action> handlePossibleActions(Map &map, const TileIndex &me, const TileIndex &target) override
        {
            if (loaded_count == 0)
                return {};

            std::vector<Action> actions{};

            // const auto join_action_option = getJoinAction(map, me, target);
            // if (join_action_option)
            //     actions.push_back(*join_action_option);

            if (map[target].unit == nullptr || target == me)
            {
                for (const auto &unit : loaded_units)
                {
                    if (!unit)
                        continue;
                    actions.push_back({.name = unit->id, .execute = [&map, this, me, target](const unsigned &index)
                                                         {
                                                             unloadUnit(map, me, target, index);
                                                         }});
                }
            }
            return actions;
        }

        std::vector<Action>
        getUnitInteractionOption(Map &map, const TileIndex &unit, const TileIndex &me) override
        {
            std::vector<Action> actions{};

            if (isUnitLoadable(*map[unit].unit))
                actions.push_back({.name = "Load", .execute = [&map, this, me, unit](const unsigned &)
                                                   {
                                                       map.game.sendAction(Load, map[unit].unit->movement_manager.getPath(), me);
                                                       executeLoadAction(map, unit, me);
                                                       //    loaded_unit->endTurn(map);
                                                   }});

            const auto possible_join_action = getJoinAction(map, unit, me);
            if (possible_join_action)
                actions.push_back(*possible_join_action);

            return actions;
        }

        virtual void displayAdditions(sf::RenderWindow &window, Map &map, const TileIndex &index) override
        {
            if (loaded_count == 0)
                return;

            star.setScale(map.scale * 0.3, map.scale * 0.3);
            star.setOrigin(0, -15);

            const auto [x, y] = map.getScreenPosition(index);
            star.setPosition(x, y);
            window.draw(star);
        }

        std::vector<std::string> loadable_units;
        std::array<std::shared_ptr<Unit>, capacity> loaded_units{};
        unsigned loaded_count{0};

        sf::Sprite star;
    };
}
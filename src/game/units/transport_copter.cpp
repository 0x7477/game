#include <game/unit.hpp>
#include <game/map.hpp>
#include <game/tile_selector.hpp>
namespace Units
{

    class TransportCopter : public Unit
    {

    public:
        TransportCopter(const Team &team)
            : Unit{team, {6, Air}}
        {
        }
        virtual bool allowUnitInteraction(const Unit &unit) override
        {
            if (loaded_unit)
                return false;

            return unit.id == "Infantry" || unit.id == "Mech";
        }

        virtual void executeUnitInteraction(Map &map, const TileIndex &unit, const TileIndex &me) override
        {
            assert(!loaded_unit); // should be disallowd by allowUnitInteraction

            loaded_unit = map.getTile(unit).unit;
            map.getTile(unit).unit = nullptr;
        }

        virtual std::vector<Action> handlePossibleActions(Map &map, const TileIndex &me, const TileIndex &target) override
        {
            if (!loaded_unit)
                return {};

            std::vector<Action> actions{};

            actions.push_back({.name = "Unload", .execute = [&map, this, me, target]()
        {
            map.moveUnit(me, target);

            const auto possible_unload_tiles = UnloadSelector::getTiles(map, target, *loaded_unit);
            map.setMovementTileMode(possible_unload_tiles, Tile::DisplayMode::Move);
            map.mode = SelectTarget;
            map.selectable_targets = possible_unload_tiles;

            map.select_function = [this, &map, me, target](const TileIndex &unload_tile)
            {
                
                //                                                     void MovementManager::init(const TileIndex &start)
// {
//     path = {start};
//     current_movement_costs = 0;
// }
// void MovementManager::startAnimation(const std::function<void()> &on_animation_finished_)
// {
//     is_moving = true;
//     on_animation_finished = on_animation_finished_;
//     delta_time.update();
//     time_passed = 0;
// }
                loaded_unit->movement_manager.init(target);
                loaded_unit->movement_manager.updatePath(map,unload_tile);
                
                loaded_unit->movement_manager.startAnimation([](){});
                map.getTile(unload_tile).unit = loaded_unit;
                map.clearTileEffects();
                loaded_unit = nullptr;
                endTurn(map);
            };
        }});

            return actions;
        }

        std::shared_ptr<Unit> loaded_unit{nullptr};

        std::vector<Action> getUnitInteractionOption(Map &map, const TileIndex &unit, const TileIndex &me) override
        {
            std::vector<Action> actions{};

            const auto unit_id = map.getTile(unit).unit->id;

            if (unit_id == "Infantry" || unit_id == "Mech")
                actions.push_back({.name = "Load", .execute = [&map, this, unit]()
                                                   {
                                                       loaded_unit = map.getTile(unit).unit;
                                                       map.getTile(unit).unit = nullptr;
                                                       loaded_unit->endTurn(map);
                                                   }});

            if (unit_id == id && health < max_health)
            {
                const auto has_other_helicopter_unit_loaded = ((TransportCopter *)map.getTile(unit).unit.get())->loaded_unit;

                if (!has_other_helicopter_unit_loaded || !loaded_unit)
                    actions.push_back({.name = "Join", .execute = [&map, this, unit]()
                                                       {
                                                           health += map.getTile(unit).unit->health;
                                                           health = std::min(health, 100);

                                                           // TODO add money
                                                           map.getTile(unit).unit = nullptr;
                                                           endTurn(map);
                                                       }});
            }

            return actions;
        }
    };
}

namespace
{
    volatile bool init = init_unit<Units::TransportCopter>();
}
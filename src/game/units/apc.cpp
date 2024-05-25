#include <game/unit.hpp>
#include <game/map.hpp>
#include <game/game.hpp>
#include <game/tile_selector.hpp>
namespace Units
{

    class APC : public Unit
    {

    public:
        APC(const Team &team)
            : Unit{team, {6,Air,0}}
        {
            actions[Unload] = [this](Map &map, const TileIndex &me, const TileIndex &new_position, const TileIndex &target){this->executeUnloadAction(map, me, new_position, target);};
        }

        virtual bool receivesTerrainBonus() const override
        {
            return false;
        }
        virtual bool allowUnitInteraction(const Unit &unit) override
        {
            if (loaded_unit)
                return false;

            return unit.id == "Infantry" || unit.id == "Mech";
        }

        virtual void executeUnitInteraction(Map &map, const TileIndex &unit, const TileIndex &me) override
        {
            if(map[unit].unit->id == id)
            {
                executeJoinAction(map, unit, me, me);
                return;
            }

            assert(!loaded_unit); // should be disallowd by allowUnitInteraction
            loaded_unit = map[unit].unit;
            map[unit].unit = nullptr;
        }

        void executeUnloadAction(Map &map, const TileIndex &me, const TileIndex &new_position, const TileIndex &target)
        {
            move(map, me, new_position);
            loaded_unit->setFinished();
            loaded_unit->movement_manager.init(new_position);
            loaded_unit->movement_manager.updatePath(map, target);

            map[target].unit = loaded_unit;
            loaded_unit->movement_manager.startAnimation([&map, target]()
                                                         { map[target].unit->movement_manager.stop(); });
            loaded_unit = nullptr;
            endTurn(map);
        }

        virtual std::vector<Action> handlePossibleActions(Map &map, const TileIndex &me, const TileIndex &target) override
        {
            if (!loaded_unit)
                return {};

            std::vector<Action> actions{};

            actions.push_back({.name = "Unload", .execute = [&map, this, me, target]()
                                                 {
                                                     const auto possible_unload_tiles = UnloadSelector::getTiles(map, target, *loaded_unit);
                                                     map.setMovementTileMode(possible_unload_tiles, Tile::DisplayMode::Move);
                                                     map.mode = SelectTarget;
                                                     map.selectable_targets = possible_unload_tiles;

                                                     map.select_function = [this, &map, me, target](const TileIndex &unload_tile) 
                                                     {
                                                        map.game.sendAction(Unload, movement_manager.getPath(), unload_tile);
                                                        map.clearTileEffects();
                                                        executeUnloadAction(map, me, target, unload_tile);
                                                     };
                                                 }});

            return actions;
        }

        std::shared_ptr<Unit> loaded_unit{nullptr};

        std::vector<Action> getUnitInteractionOption(Map &map, const TileIndex &unit, const TileIndex &me) override
        {
            std::vector<Action> actions{};

            const auto unit_id = map[unit].unit->id;

            if (unit_id == "Infantry" || unit_id == "Mech")
                actions.push_back({.name = "Load", .execute = [&map, this, me, unit]()
                                                   {
                                                       map.game.sendAction(Load, map[unit].unit->movement_manager.getPath(), me);
                                                       executeLoadAction(map, unit, me, me);
                                                       //    loaded_unit->endTurn(map);
                                                   }});

            const auto possible_join_action = getJoinAction(map, unit, me);
            if(possible_join_action)
                actions.push_back(*possible_join_action);

            return actions;
        }
    };
}

namespace
{
    volatile bool init = init_unit<Units::APC>();
}
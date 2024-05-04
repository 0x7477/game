#include <game/unit.hpp>
#include <game/map.hpp>
#include <game/game.hpp>
namespace Units
{
    class Infantry : public Unit
    {
    public:
        Infantry(const Team &team)
            : Unit{team, {3, Foot}}
        {
            actions[Capture] = [this](Map &map, const TileIndex &me, const TileIndex &new_position, const TileIndex &target){this->executeCaptureAction(map, me, new_position, target);};
        }

        virtual void onMoved() override
        {
            capture_progress = 0;
        }


        void executeCaptureAction(Map &map, const TileIndex &me, const TileIndex &new_position, const TileIndex &target)
        {
            if(capturing_tile_index && *capturing_tile_index != target)
                    capture_progress = 0;

            capture_progress += getUnitCount();
            move(map, me, target);
            if (capture_progress >= 20)
                map[target].setTeam(team);

            endTurn(map);
        }

        virtual std::vector<Action> handlePossibleActions(Map &map,const TileIndex &me, const TileIndex &target) override
        {
            std::vector<Action> actions{};
            if (map[target].getId() == "Base" && map[target].team != map.team)
                actions.push_back(Action{.name = "Capture", .execute = [&map, me, target, this]()
                {
                    executeCaptureAction(map, me, target, target);
                    map.game.sendAction(Capture, movement_manager.getPath(), target);
                }});

            return actions;
        }

        std::optional<TileIndex> capturing_tile_index{};
        unsigned capture_progress{0};
    };

}
namespace
{
    volatile bool init = init_unit<Units::Infantry>();
}
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


        void executeCaptureAction(Map &map, const TileIndex &me, const TileIndex &, const TileIndex &target)
        {
            move(map, me, target);
            capture_progress += getUnitCount();

            if (capture_progress >= 20)
            {
                map[target].setTeam(team);
                map.win(team);
            }

            endTurn(map);
        }

        virtual std::vector<Action> handlePossibleActions(Map &map,const TileIndex &me, const TileIndex &target) override
        {
            std::vector<Action> actions{};
            if ((  map[target].getId() == "Base" 
                || map[target].getId() == "City" 
                || map[target].getId() == "Port" 
                || map[target].getId() == "Headquarter" 
                || map[target].getId() == "Lab" 
                || map[target].getId() == "CommunicationTower" 
                || map[target].getId() == "Airport") && map[target].team != map.team)
                actions.push_back(Action{.name = "Capture", .execute = [&map, me, target, this]()
                {
                    executeCaptureAction(map, me, target, target);
                    map.game.sendAction(Capture, movement_manager.getPath(), target);
                }});

            return actions;
        }

        unsigned capture_progress{0};
    };

}
namespace
{
    volatile bool init = init_unit<Units::Infantry>();
}
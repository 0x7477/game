#include <game/unit.hpp>
#include <game/map.hpp>
#include <game/game.hpp>
namespace Units
{
    class Trooper : public Unit
    {
    public:
        Trooper(const Team &team, const Stats &stats, const std::source_location &location = std::source_location::current())
            : Unit{team, stats, location}
        {
            actions[Capture] = [this](Map &map, const TileIndex &me, const TileIndex &new_position, const TileIndex &, const unsigned&){this->executeCaptureAction(map, me, new_position);};
        }

        virtual void onMoved() override
        {
            capture_progress = 0;
        }


        void executeCaptureAction(Map &map, const TileIndex &me, const TileIndex &target)
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
                actions.push_back(Action{.name = "Capture", .execute = [&map, me, target, this](const unsigned&)
                {
                    executeCaptureAction(map, me, target);
                    map.game.sendAction(Capture, movement_manager.getPath(), target);
                }});

            return actions;
        }

        unsigned capture_progress{0};
    };

}
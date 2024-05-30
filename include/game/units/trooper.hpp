#include <game/unit.hpp>
#include <game/map.hpp>
#include <game/game.hpp>
#include <game/map.hpp>
namespace Units
{
    class Trooper : public Unit
    {
    public:
        Trooper(const Team &team, const Stats &stats, const std::source_location &location = std::source_location::current())
            : Unit{team, stats, location}, flag{image_resources.get("misc/flag.png")}
        {
            actions[Capture] = [this](Map &map, const TileIndex &me, const TileIndex &new_position, const TileIndex &, const unsigned &)
            { this->executeCaptureAction(map, me, new_position); };
        }

        virtual void onMoved() override
        {
            capture_progress = 0;
        }

        virtual void displayAdditions(sf::RenderWindow &window, Map &map, const TileIndex &index) override
        {
            if (capture_progress == 0 || map[index].team == team)
                return;

            const std::string current_team = std::to_string(map[index].team);
            const std::string my_team = std::to_string(team);
            const std::string cur_capture_progress = std::to_string(capture_progress / 2);

            flag.setScale(map.scale * 0.5, map.scale* 0.5);
            flag.setOrigin(0,-15);

            flag.setTexture(image_resources.get("misc/flags/flag_" + current_team + "_" + my_team + "_" + cur_capture_progress + ".png"));
            const auto [x, y] = map.getScreenPosition(index);
            flag.setPosition(x, y);
            window.draw(flag);
        }

        void executeCaptureAction(Map &map, const TileIndex &me, const TileIndex &target)
        {
            move(map, me, target);
            capture_progress += getUnitCount();

            if (capture_progress >= 20)
            {
                map[target].setTeam(team);

                if (map[target].getId() == "Headquarter")
                    map.win(team);
            }

            endTurn(map);
        }

        virtual std::vector<Action> handlePossibleActions(Map &map, const TileIndex &me, const TileIndex &target) override
        {
            std::vector<Action> actions{};
            if ((map[target].getId() == "Base" || map[target].getId() == "City" || map[target].getId() == "Port" || map[target].getId() == "Headquarter" || map[target].getId() == "Lab" || map[target].getId() == "CommunicationTower" || map[target].getId() == "Airport") && map[target].team != map.team)
                actions.push_back(Action{.name = "Capture", .execute = [&map, me, target, this](const unsigned &)
                                                            {
                                                                executeCaptureAction(map, me, target);
                                                                map.game.sendAction(Capture, movement_manager.getPath(), target);
                                                            }});

            return actions;
        }

        sf::Sprite flag;
        unsigned capture_progress{0};
    };

}
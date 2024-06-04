#include <ui/attack_info.hpp>
#include <game/map.hpp>

UI::AttackInfo::AttackInfo()
    : text{"", font_resources.get("arial.ttf")}, background{image_resources.get("misc/unit_detail.png")}
{
    const float x{150};
    const float y{40};
    // text.setFillColor(sf::Color::Black);
    background.scale(x / (float)background.getTexture()->getSize().x, y / (float)background.getTexture()->getSize().y);
}

void UI::AttackInfo::draw(sf::RenderWindow &window)
{
    if (!show)
        return;
    const auto pos_x = WindowManager::window_width - 160;
    const auto pos_y = WindowManager::window_height / 2 - 80;

    background.setPosition(pos_x, pos_y);
    background.setColor(sf::Color::Red);
    window.draw(background);
    text.setPosition(pos_x + 15, pos_y);
    text.setString(std::to_string(attack_info.attack_min) + "% - " + std::to_string(attack_info.attack_max) + "%");
    window.draw(text);

    if (!attack_info.can_counterattack)
        return;
    background.setPosition(pos_x, pos_y + 45);
    background.setColor(sf::Color::Blue);
    window.draw(background);
    text.setPosition(pos_x + 15, pos_y + 45);
    text.setString(std::to_string(attack_info.defender_min) + "% - " + std::to_string(attack_info.defender_max) + "%");
    window.draw(text);
}
void UI::AttackInfo::update(Map &map, const TileIndex &index)
{
    if (index == last_index)
        return;

    last_index = index;

    if (std::find(map.selectable_targets->begin(), map.selectable_targets->end(), index) == map.selectable_targets->end())
    {
        show = false;
        return;
    }

    if(map[index].unit == nullptr)
    {
        show = true;
        attack_info.attack_min = map[index].getPotentialDamageFrom(*(map[*map.selected_unit].unit));
        attack_info.attack_max = attack_info.attack_min;
        attack_info.can_attack = true;
        attack_info.can_counterattack = false;
        return;
    }

    assert(map.selected_unit.has_value());
    show = map[index].unit != nullptr && map[index].unit->getTeam() != map[*map.selected_unit].unit->getTeam();

    if (!show)
        return;

    const auto new_pos = map[*map.selected_unit].unit->movement_manager.getEndPosition();
    attack_info = AttackSimulator::calculatePossibleDamageValues(map, *map.selected_unit, new_pos, index);
    if (!attack_info.can_attack)
        show = false;
}

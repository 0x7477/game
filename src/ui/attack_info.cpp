#include <ui/attack_info.hpp>
#include <game/map.hpp>

UI::AttackInfo::AttackInfo()
    : text{"", font_resources.get("arial.ttf")},background{image_resources.get("misc/unit_detail.png")}
{
    const float x{150};
    const float y{40};
    // text.setFillColor(sf::Color::Black);
    background.scale(x / (float)background.getTexture()->getSize().x, y / (float)background.getTexture()->getSize().y);
}


void UI::AttackInfo::draw(sf::RenderWindow &window)
{
    if(!show)
        return;
    const auto pos_x = WindowManager::window_width - 160;
    const auto pos_y = WindowManager::window_height / 2 - 80;
    
    background.setPosition(pos_x, pos_y);
    background.setColor(sf::Color::Red);
    window.draw(background);
    text.setPosition(pos_x + 15, pos_y);
    text.setString(std::to_string(attack_info.attack_min) + "% - " + std::to_string(attack_info.attack_max) + "%"); 
    window.draw(text);

    
    if(!attack_info.can_counterattack)
        return;
    background.setPosition(pos_x , pos_y+45);
    background.setColor(sf::Color::Blue);
    window.draw(background);
    text.setPosition(pos_x  + 15, pos_y+45);
    text.setString(std::to_string(attack_info.defender_min) + "% - " + std::to_string(attack_info.defender_max) + "%"); 
    window.draw(text);


}
void UI::AttackInfo::update(Map& map, const TileIndex &index)
{
    if(index == last_index)
        return;
    last_index = index;
    
    assert(map.selected_unit.has_value());
    show = map[index].unit != nullptr && map[index].unit->getTeam() != map[*map.selected_unit].unit->getTeam();

    if(!show)
        return;

    attack_info = AttackSimulator::calculatePossibleDamageValues(map, *map.selected_unit, index);
    

}

#include <ui/parts_menu/ring.hpp>
#include <helper/resource_loader.hpp>
UI::PartsMenuRing::PartsMenuRing(const unsigned int &layer, const std::vector<std::variant<PartsMenuCategory, PartsMenuElement>> &children)
    : radius{layer * 80.f + 30}, thickness{50}, children{children}
{
}

void UI::PartsMenuRing::executeChildFunction()
{
    if(selected_index < 0) return; 
    auto &child = children[selected_index];

    if (std::holds_alternative<PartsMenuCategory>(child))
        std::get<PartsMenuCategory>(child).executeChildFunction();
    else
        std::get<PartsMenuElement>(child).executeChildFunction();
}

void UI::PartsMenuRing::draw(sf::RenderWindow &window, const float &pos_x, const float &pos_y)
{
    const auto mouse_position = sf::Mouse::getPosition(window);
    const auto segment_angle_size = 2 * M_PI / children.size();

    sf::CircleShape menu(radius + thickness);
    menu.setPosition(pos_x - radius - thickness, pos_y - radius - thickness);
    menu.setFillColor(sf::Color::Transparent);
    menu.setOutlineColor(sf::Color::White);
    menu.setOutlineThickness(3);
    window.draw(menu);
    menu.setRadius(radius);
    menu.setPosition(pos_x - radius, pos_y - radius);
    window.draw(menu);

    sf::RectangleShape seperator({thickness, 1});
    seperator.setFillColor(sf::Color::White);
    auto current_angle{-M_PI / 2};

    const auto dx = mouse_position.x - pos_x;
    const auto dy = mouse_position.y - pos_y;
    const auto length = std::sqrt(dx * dx + dy * dy);

    if (length > radius && length < radius + thickness)
    {
        const auto angle = fmodf(3 * M_PI + std::atan2(dy, dx) - M_PI / 2, 2 * M_PI);
        const auto offset_angle = angle + M_PI / children.size();

        const int new_selected_index = int(children.size() + (children.size() * offset_angle) / (2 * M_PI)) % children.size();

        if (selected_index != new_selected_index)
        {
            selected_index = new_selected_index;
            executeChildFunction();
        }

    }

    int index = 0;
    for (auto &child : children)
    {
        const auto seperator_angle = ((current_angle - segment_angle_size / 2) / M_PI) * 180;
        seperator.setPosition(pos_x + radius * std::cos(current_angle - segment_angle_size / 2), pos_y + radius * std::sin(current_angle - segment_angle_size / 2));
        seperator.setRotation(seperator_angle);
        window.draw(seperator);

        const auto child_distance = radius + 0.5 * thickness;

        const auto child_pos_x = pos_x + child_distance * cos(current_angle);
        const auto child_pos_y = pos_y + child_distance * sin(current_angle);
        if (std::holds_alternative<PartsMenuCategory>(child))
            std::get<PartsMenuCategory>(child).draw(window, pos_x, pos_y, child_pos_x, child_pos_y, selected_index == index);
        else
            std::get<PartsMenuElement>(child).draw(window, child_pos_x, child_pos_y, selected_index == index);

        current_angle += segment_angle_size;
        index++;
    }
}

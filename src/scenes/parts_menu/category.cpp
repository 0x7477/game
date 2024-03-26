#include <ui/parts_menu/category.hpp>
#include <ui/parts_menu/ring.hpp>

UI::PartsMenuCategory::PartsMenuCategory(const unsigned int &layer, const std::string &text, const std::vector<std::variant<PartsMenuCategory, PartsMenuElement>> &children)
    : children{children}, text{text, font_resources.get("arial.ttf"), font_size}, ring{new PartsMenuRing{layer, children}}
{
}

void UI::PartsMenuCategory::executeChildFunction()
{
    ring->executeChildFunction();
}

void UI::PartsMenuCategory::draw(sf::RenderWindow &window, const float &center_x, const float &center_y, const float &pos_x, const float &pos_y, const bool &selected)
{
    if (selected)
        ring->draw(window, center_x, center_y);

    text.setFillColor(sf::Color::White);

    sf::FloatRect textRect = text.getLocalBounds();
    text.setOrigin(textRect.left + textRect.width / 2.0f,
                   textRect.top + textRect.height / 2.0f);
    text.setPosition(pos_x, pos_y);
    text.setFillColor(selected ? sf::Color::Red : sf::Color::White);
    window.draw(text);
}

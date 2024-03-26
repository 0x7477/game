#include <ui/parts_menu/element.hpp>

UI::PartsMenuElement::PartsMenuElement(const std::string &text, const std::function<void()> &select)
    : on_selected{select},text{text, font_resources.get("arial.ttf"), 10}
{
    this->text.setFillColor(sf::Color::White);
}

void UI::PartsMenuElement::executeChildFunction()
{
    on_selected();
}

void UI::PartsMenuElement::draw(sf::RenderWindow &window, const float &pos_x, const float &pos_y, const bool &selected)
{
    text.setPosition(pos_x, pos_y);
    sf::FloatRect textRect = text.getLocalBounds();
    text.setOrigin(textRect.left + textRect.width / 2.0f,
                   textRect.top + textRect.height / 2.0f);

    text.setFillColor(selected ? sf::Color::Red : sf::Color::White);

    window.draw(text);
}

#include <ui/shopping_menu.hpp>

UI::ShoppingMenu::ShoppingMenu()
    : layout{}, unit_text{"", font_resources.get("arial.ttf")}, price_text{"", font_resources.get("arial.ttf")}, background{image_resources.get("misc/menu.png")}
{
    price_text.setFillColor(sf::Color::Black);
}

bool UI::ShoppingMenu::canBuyUnit(const std::string &unit)
{
    return money >= Unit::unit_costs[unit];
}

void UI::ShoppingMenu::draw(sf::RenderWindow &window)
{
    if (WindowManager::getKeyDown(sf::Keyboard::X))
    {
        on_cancel();
        return;
    }

    if (WindowManager::getKeyDown(sf::Keyboard::Y) && canBuyUnit(options[current_index]))
    {
        on_value_selected(current_index);
        return;
    }

    if (WindowManager::getKeyDown(sf::Keyboard::Down))
        current_index = (current_index + 1) % options.size();

    if (WindowManager::getKeyDown(sf::Keyboard::Up))
        current_index = (current_index - 1 + options.size()) % options.size();


    const auto texture_size = background.getTexture()->getSize();

    const unsigned max_width = 500;
    const float wanted_x = std::min(max_width,WindowManager::window_width / 2);
    const unsigned max_height = 400;
    const float wanted_y = std::min(WindowManager::window_height - 200u, max_height);

    const auto start_pos = (WindowManager::window_height - wanted_y)/2;
    background.setPosition(100, start_pos);

    background.setScale(wanted_x / texture_size.x, wanted_y / texture_size.y);
    window.draw(background);

    for (auto i{0u}; i < options.size(); i++)
    {
        const auto unit_cost = Unit::unit_costs[options[i]];

        unit_text.setFillColor(sf::Color::Black);
        if (money < unit_cost)
            unit_text.setFillColor(sf::Color(200, 200, 200));

        if (i == current_index)
            unit_text.setFillColor(sf::Color::Red);

        price_text.setString(std::to_string(unit_cost));
        price_text.setPosition(100 + wanted_x - 50 -price_text.getGlobalBounds().width, start_pos + i * 30);
        unit_text.setPosition(120, start_pos + i * 30);
        unit_text.setString(options[i]);
        window.draw(unit_text);
        window.draw(price_text);
    }
}

void UI::ShoppingMenu::setOptions(const unsigned &money_, const std::vector<std::string> &new_options, const std::function<void(const std::size_t &)> &on_value_selected_function, const std::function<void()> &on_cancel_function)
{
    money = money_;
    options = new_options;
    on_value_selected = on_value_selected_function;
    on_cancel = on_cancel_function;
    current_index = 0;
}

#include <ui/parts_menu.hpp>


#define P(NAME,TYPE) PartsMenuElement{NAME, [this](){set_id(TYPE);}}

UI::PartsMenu::PartsMenu(const Layout &layout,const std::function<void(const std::string&)>& on_value_change)
    : layout{layout}, ring{0,{
                            P("HULL", "Hull"),
                            PartsMenuCategory{1,"WEAPONS", {PartsMenuCategory{2,"LASER",{P("SMALL","SmallLaser"),P("MEDIUM","MediumLaser"),P("BIG","BigLaser")}}, P("MISSILES","Missiles"), P("DRONE","Drone")}},
                            PartsMenuCategory{1,"ENERGY", {P("REACTOR","Reactor"), P("BIG REACTOR","BigReactor")}},
                            PartsMenuCategory{1,"THRUST", {P("SMALL","SmallThrusts"), P("MEDIUM","MediumThrusts"), P("BIG","BigThrusts")}}}},
                            on_value_change{on_value_change}
{
}

void UI::PartsMenu::set_id(const std::string &s)
        {
            if (s == value)
                return;
            value = s;
            on_value_change(s);
        }

void UI::PartsMenu::draw(sf::RenderWindow &window)
{
    const auto mouse_position = sf::Mouse::getPosition(window);
    if (!drawing && !layout.getRect().contains(mouse_position.x, mouse_position.y))
        return;
    if (!sf::Mouse::isButtonPressed(sf::Mouse::Button::Right))
    {
        drawing = false;
        return;
    }

    drawing = true;
    const auto &rect = layout.getRect();

    const auto dx = mouse_position.x - rect.width / 2;
    const auto dy = mouse_position.y - rect.height / 2;
    const auto length = std::sqrt(dx * dx + dy * dy);

    sf::RectangleShape line({length, 1.f});
    line.setFillColor(sf::Color::Black);
    line.setPosition(rect.width / 2, rect.height / 2);
    line.setRotation((std::atan2(dy, dx) / M_PI) * 180);

    ring.draw(window, rect.width / 2, rect.height / 2);
    window.draw(line);
}

UI::Layout &UI::PartsMenu::getLayout()
{
    return layout;
}

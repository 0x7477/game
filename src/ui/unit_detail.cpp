#include <ui/unit_detail.hpp>
#include <game/tile.hpp>

UI::UnitDetail::UnitDetail()
    : text{"", font_resources.get("arial.ttf")}, background{image_resources.get("misc/unit_detail.png")},
      health_symbol{image_resources.get("misc/heart.png")}, shield_symbol{image_resources.get("misc/shield.png")}, ammo_symbol{image_resources.get("misc/ammo.png")}
{
    const float size = 20;
    text.setFillColor(sf::Color::Black);
    health_symbol.scale(size / (float)health_symbol.getTexture()->getSize().x, size / (float)health_symbol.getTexture()->getSize().y);
    shield_symbol.scale(size / (float)shield_symbol.getTexture()->getSize().x, size / (float)shield_symbol.getTexture()->getSize().y);
    ammo_symbol.scale(size / (float)ammo_symbol.getTexture()->getSize().x, size / (float)ammo_symbol.getTexture()->getSize().y);
}

void UI::UnitDetail::draw(sf::RenderWindow &window)
{
    const float width = 70;
    const float height = 30 + 32 * ((int)display_ammo + (int)display_health);

    const auto texture_size = background.getTexture()->getSize();

    const auto pos_x = WindowManager::window_width - 100;
    const auto pos_y = WindowManager::window_height - 30 - height;
    background.setPosition(pos_x, pos_y);
    background.setScale(width / (float)texture_size.x, height / (float)texture_size.y);

    window.draw(background);

    shield_symbol.setPosition(pos_x + 5, pos_y + 7);
    window.draw(shield_symbol);
    text.setString(protection);
    text.setPosition(pos_x + 60- text.getGlobalBounds().width, pos_y - 7);
    window.draw(text);

    if (display_health)
    {
        health_symbol.setPosition(pos_x + 5, pos_y + 37);
        window.draw(health_symbol);
        text.setString(health);
        text.setPosition(pos_x + 60 - text.getGlobalBounds().width, pos_y - 7 + 30);
        window.draw(text);
    }

    if (display_ammo)
    {
        ammo_symbol.setPosition(pos_x + 5, pos_y + 67);
        window.draw(ammo_symbol);
        text.setString(ammo);
        text.setPosition(pos_x + 60- text.getGlobalBounds().width, pos_y - 7 + 60);
        window.draw(text);
    }
}

void UI::UnitDetail::setInfo(const Tile &tile)
{
    protection = std::to_string(tile.getDefense());
    display_health = false;
    if (tile.unit)
    {
        ammo = std::to_string(tile.unit->ammo);
        display_ammo = tile.unit->stats.max_ammo > 0;
        health = std::to_string(tile.unit->getUnitCount());
        display_health = true;
    }
    else
    {
        if(tile.getHealth())
        {
            health = std::to_string(*tile.getHealth());
            display_health = true;
        }
        display_ammo = false;
    }
}

#include <game/tile.hpp>
#include <game/map.hpp>

void Tile::display(sf::RenderWindow &window, const Map &map, const TileIndex &index)
{
    const auto [x, y] = map.getScreenPosition(index);

    // sprite.setPosition(x, y);
    // sprite.setScale(map.scale, map.scale);

    // sprite.setOrigin(0, sprite.getTexture()->getSize().y - 16);

    // sprite.setColor(display_mode == Move ? sf::Color::Blue : (display_mode == Attack ? sf::Color::Red : sf::Color::White));

    sprite.display(window, x,y - map.scale *(sprite.getTexture()->getSize().y - 16),map.scale);

    if(display_mode == Move)
        movement_effect.display(window, x,y,map.scale);
    if(display_mode == Attack)
        attack_effect.display(window, x,y,map.scale);

    // window.draw();
}

void Tile::displayUnit(sf::RenderWindow &window, const Map &map, const TileIndex &index)
{
    if (unit)
        unit->display(window, map, index);
}

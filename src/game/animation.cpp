#include <game/animation.hpp>
#include <game/map.hpp>
Animation::Animation(const UI::GIF& gif_, const float &ttl,const TileIndex& index, const unsigned& anchor_x,const unsigned& anchor_y)
    :index{index}, ttl{ttl}, gif{gif_}, start_point{std::chrono::system_clock::now()}
{
    gif.animation.spritesheet.setOrigin(anchor_x,anchor_y);
}

bool Animation::draw(sf::RenderWindow &window, Map &map)
{
    auto now = std::chrono::system_clock::now();
    std::chrono::duration<float> elapsed = now - start_point;

    if (elapsed.count() > ttl)
        return false;

    const auto [x, y] = map.getScreenPosition(index);

    gif.display(window, x, y, map.scale);

    return true;
}

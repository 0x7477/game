#pragma once
#include <string>
#include <sstream>
#include <span>
#include <vector>
#include <SFML/Graphics.hpp>
#include <helper/resource_loader.hpp>
#include <game/tile.hpp>
#include <cassert>
#include <engine/window_manager.hpp>
#include <helper/delta_timer.hpp>
#include <game/unit.hpp>
#include <ui/shopping_menu.hpp>
#include <ui/action_menu.hpp>
#include <game/visualization_mode.hpp>
#include <game/tile_index.hpp>

class Game;


class Map
{
public:
    Map(Game& game, const std::string &data_string)
        : game{game}, cursor_sprite{gif_resources.get("unit_select.gif")},
          tiles{initTiles(data_string)}, width{(unsigned)tiles[0].size()}, height{(unsigned)tiles.size()}
    {
        (*this)[4, 4].unit = Unit::createUnit("Infantry", Team::Blue);
        (*this)[5, 4].unit = Unit::createUnit("TransportCopter", Team::Red);
        (*this)[6, 4].unit = Unit::createUnit("Infantry", Team::Red);;
    }

    std::vector<std::vector<unsigned>> getTileIds(const std::string &data_string);

    std::vector<std::vector<Tile>> initTiles(const std::string &data_string);

    std::vector<unsigned> getLineData(const std::string_view &line);

    template <ViewMode mode>
    void displayMode(sf::RenderWindow &window);

    void display(sf::RenderWindow &window);

    void drawCursor(sf::RenderWindow &window);

    Tile &getCursorTile();

    Tile &getTile(const TileIndex &index);

    void handleEvents();

    std::vector<TileIndex> getNeighbors(const TileIndex &index)
    {
        std::vector<TileIndex> ret;

        if (index.x > 0)
            ret.push_back({index.x - 1, index.y});
        if (index.x < width - 1)
            ret.push_back({index.x + 1, index.y});
        if (index.y > 0)
            ret.push_back({index.x, index.y - 1});
        if (index.y < height - 1)
            ret.push_back({index.x, index.y + 1});

        return ret;
    }

    void endTurn()
    {
        for (unsigned y{0}; y < height; y++)
            for (unsigned x{0}; x < width; x++)
                if (tiles[y][x].unit)
                    tiles[y][x].unit->startRound(*this, {x,y});
    }

    void handleMenu();

    void beginTurn()
    {
        for (unsigned y{0}; y < height; y++)
            for (unsigned x{0}; x < width; x++)
                if (tiles[y][x].unit)
                    tiles[y][x].unit->startRound(*this, {x, y});
                
    }

    void killUnit(Tile &tile)
    {
        tile.unit = nullptr;
    }

    void drawMap(sf::RenderWindow &window)
    {
        for (unsigned y{0}; y < height; y++)
            for (unsigned x{0}; x < width; x++)
                tiles[y][x].display(window, *this, {x, y});

        for (unsigned y{0}; y < height; y++)
            for (unsigned x{0}; x < width; x++)
                tiles[y][x].displayUnit(window, *this, {x, y});
    }

    void moveCursorInDirection(unsigned &cursor_var, const int delta, float &time_since_movement, const unsigned &max_value)
    {
        constexpr float time_between_scrolls{0.1};
        constexpr float init_scroll_wait{0.3};
        if (time_since_movement_button_was_pressed == 0)
        {
            cursor_var = std::max(0, std::min((int)max_value - 1, (int)cursor_var + delta));
            time_since_movement = 0;
        }
        else if (time_since_movement_button_was_pressed >= init_scroll_wait)
        {
            if (time_since_movement > time_between_scrolls)
            {
                cursor_var = std::max(0, std::min((int)max_value - 1, (int)cursor_var + delta));
                time_since_movement = 0;
            }
            else
            {
                time_since_movement += delta_time;
            }
        }
    }

    void moveCursor()
    {
        auto &[cursor_x, cursor_y] = cursor;
        delta_time.update();
        bool button_was_pressed{false};
        if (WindowManager::getKey(sf::Keyboard::Left) && !WindowManager::getKey(sf::Keyboard::Right))
        {
            button_was_pressed = true;
            moveCursorInDirection(cursor_x, -1, time_since_last_left_move, width);
        }
        if (WindowManager::getKey(sf::Keyboard::Right) && !WindowManager::getKey(sf::Keyboard::Left))
        {
            button_was_pressed = true;
            moveCursorInDirection(cursor_x, 1, time_since_last_right_move, width);
        }
        if (WindowManager::getKey(sf::Keyboard::Up) && !WindowManager::getKey(sf::Keyboard::Down))
        {
            button_was_pressed = true;

            moveCursorInDirection(cursor_y, -1, time_since_last_up_move, height);
        }
        if (WindowManager::getKey(sf::Keyboard::Down) && !WindowManager::getKey(sf::Keyboard::Up))
        {
            button_was_pressed = true;
            moveCursorInDirection(cursor_y, +1, time_since_last_down_move, height);
        }

        if (!button_was_pressed)
            time_since_movement_button_was_pressed = 0;
        else
            time_since_movement_button_was_pressed += delta_time;
    }

    std::shared_ptr<Unit> createUnit(const std::string &id, const Team &team_id, const TileIndex &position)
    {
        return createUnit(id, team_id, getTile(position));
    }

    std::shared_ptr<Unit> createUnit(const std::string &id, const Team &team_id, Tile &tile)
    {
        tile.unit = Unit::createUnit(id, team_id);
        return tile.unit;
    }

    std::shared_ptr<Unit> createUnit(const std::shared_ptr<Unit> &unit, const TileIndex &position)
    {
        (*this)[position].unit = unit;
        return unit;
    }

    void clearTileEffects()
    {
        for (unsigned y{0}; y < tiles.size(); y++)
            for (unsigned x{0}; x < tiles[y].size(); x++)
                tiles[y][x].setDisplayMode(Tile::DisplayMode::Normal);
    }

    void setMovementTileMode(const std::vector<TileIndex> &indices, const Tile::DisplayMode &mode)
    {
        for (const auto &index : indices)
            getTile(index).setDisplayMode(mode);
    }

    std::tuple<float, float> getScreenPosition(const TileIndex &index) const
    {
        return {index.x * 16 * scale, index.y * 16 * scale};
    }

    Tile &operator[](const TileIndex &index)
    {
        return tiles[index.y][index.x];
    }

    Tile &operator[](const unsigned &x, const unsigned &y)
    {
        return tiles[y][x];
    }

    Game& game;
    sf::Sprite cursor_sprite;
    std::vector<std::vector<Tile>> tiles{};
    unsigned width, height;
    Team team{Red};
    // float pos_x, pos_y;
    TileIndex cursor{1, 1};
    std::optional<TileIndex> selected_unit{};

    std::optional<std::vector<TileIndex>> selectable_targets{};
    std::function<void(const TileIndex &)> select_function;
    const float scale = 4;
    constexpr static float tile_size{16.f};
    float time_since_movement_button_was_pressed;

    float time_since_last_left_move{0};
    float time_since_last_right_move{0};
    float time_since_last_up_move{0};
    float time_since_last_down_move{0};

    UI::ShoppingMenu shopping_menu{};
    UI::ActionMenu action_menu{};
    ViewMode mode{View};
    DeltaTimer delta_time{};
};
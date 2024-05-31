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
#include <ui/shopping_menu.hpp>
#include <ui/action_menu.hpp>
#include <game/visualization_mode.hpp>
#include <game/tile_index.hpp>
#include <ui/endcard.hpp>
#include <ui/unit_detail.hpp>
#include <ui/header.hpp>
#include <list>
#include <game/animation.hpp>
#include <helper/delta_timer.hpp>
#include <ui/attack_info.hpp>

class Game;
class Unit;

class Map
{
public:
    Map(Game &game, const std::string &data_string);

    std::vector<std::vector<unsigned>> getTileIds(const std::string &data_string);

    std::vector<std::vector<std::unique_ptr<Tile>>> initTiles(const std::string &data_string);

    std::vector<unsigned> getLineData(const std::string_view &line);

    template <ViewMode mode>
    void displayMode(sf::RenderWindow &window);

    void display(sf::RenderWindow &window);

    void drawCursor(sf::RenderWindow &window);

    void resize();
    Tile &getCursorTile();

    Tile &getTile(const TileIndex &index);

    void handleEvents();

    std::vector<TileIndex> getNeighbors(const TileIndex &index);

    void endTurn();

    void handleMenu();

    void beginTurn(const Team &begin_turn_team);

    void drawMap(sf::RenderWindow &window);
    void drawAnimations(sf::RenderWindow &window);

    void moveCursorInDirection(unsigned &cursor_var, const int delta, float &time_since_movement, const unsigned &max_value);

    void moveMapToContain(const TileIndex &index);

    void moveCursor();

    bool hasTeamNoUnitsLeft(const Team &team);

    void win(const Team &winner_team);

    void killUnit(const TileIndex &position);

    std::shared_ptr<Unit> createUnit(const std::string &id, const Team &team_id, const TileIndex &position);

    std::shared_ptr<Unit> createUnit(const std::string &id, const Team &team_id, Tile &tile);

    void clearTileEffects();

    void setMovementTileMode(const std::vector<TileIndex> &indices, const Tile::DisplayMode &mode);

    std::tuple<float, float> getScreenPosition(const TileIndex &index) const;

    std::tuple<float, float> getUnsmoothScreenPosition(const TileIndex &index) const;

    Tile &operator[](const TileIndex &index);

    Tile &operator[](const unsigned &x, const unsigned &y);


    Game &game;
    sf::Sprite cursor_sprite;
    std::vector<std::vector<std::unique_ptr<Tile>>> tiles{};
    unsigned width, height;
    Team team{Red};
    Team winner{Neutral};
    float pos_x{0}, pos_y{0};
    float smooth_x{0}, smooth_y{0};
    float smooth_cursor_x{0}, smooth_cursor_y{0};
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
    UI::EndCard endcard;
    UI::UnitDetail unit_detail;
    UI::Header header;
    UI::AttackInfo attack_info;
    ViewMode mode{View};
    DeltaTimer delta_time{};

    std::list<Animation> animations;
};
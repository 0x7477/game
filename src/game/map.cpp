#include <game/map.hpp>
#include <game/game.hpp>
#include <scenes/battle.hpp>
#include <game/map_manipulator.hpp>
#include <game/tile_selector.hpp>
Map::Map(Game &game, const std::string &data_string)
    : game{game}, cursor_sprite{gif_resources.get("unit_select.gif")},
      tiles{initTiles(data_string)}, width{(unsigned)tiles[0].size()}, height{(unsigned)tiles.size()}
{
    resize();
    beginTurn(team);
    // (*this)[0, 0].unit = Unit::createUnit(ANTIAIR, Team::Blue);
    // (*this)[0, 1].unit = Unit::createUnit(APC, Team::Blue);
    // (*this)[0, 2].unit = Unit::createUnit(ARTILLERY, Team::Blue);
    // (*this)[0, 3].unit = Unit::createUnit(BATTLECOPTER, Team::Blue);
    // (*this)[0, 4].unit = Unit::createUnit(BATTLESHIP, Team::Blue);
    // // (*this)[1, 0].unit = Unit::createUnit(BLACKBOAT, Team::Blue);
    // (*this)[1, 1].unit = Unit::createUnit(BOMBER, Team::Blue);
    // // (*this)[1, 2].unit = Unit::createUnit(CARRIER, Team::Blue);
    // (*this)[1, 3].unit = Unit::createUnit(CRUISER, Team::Blue);
    // (*this)[1, 4].unit = Unit::createUnit(FIGHTER, Team::Blue);
    // (*this)[2, 0].unit = Unit::createUnit(INFANTRY, Team::Blue);
    // (*this)[2, 1].unit = Unit::createUnit(LANDER, Team::Blue);
    // (*this)[2, 2].unit = Unit::createUnit(MEDIUMTANK, Team::Blue);
    // (*this)[2, 3].unit = Unit::createUnit(MECH, Team::Blue);
    // (*this)[3, 0].unit = Unit::createUnit(MEGATANK, Team::Blue);
    // (*this)[3, 1].unit = Unit::createUnit(MISSILE, Team::Blue);
    // (*this)[3, 2].unit = Unit::createUnit(NEOTANK, Team::Blue);
    // (*this)[3, 3].unit = Unit::createUnit(PIPERUNNER, Team::Blue);
    // (*this)[3, 4].unit = Unit::createUnit(RECON, Team::Blue);
    // (*this)[4, 0].unit = Unit::createUnit(ROCKET, Team::Blue);
    // // (*this)[4, 1].unit = Unit::createUnit(STEALTH, Team::Blue);
    // // (*this)[4, 2].unit = Unit::createUnit(SUB, Team::Blue);
    // (*this)[4, 3].unit = Unit::createUnit(TCOPTER, Team::Blue);
    // (*this)[4, 4].unit = Unit::createUnit(TANK, Team::Blue);

    // (*this)[4, 4].unit = Unit::createUnit("Infantry", Team::Blue);
    // (*this)[5, 4].unit = Unit::createUnit("TransportCopter", Team::Red);
    // (*this)[6, 4].unit = Unit::createUnit("Infantry", Team::Red);
    // (*this)[6, 4].unit->heal(-80);

    (*this)[8, 4].unit = Unit::createUnit("Infantry", Team::Blue);

    (*this)[7, 4].unit = Unit::createUnit("Infantry", Team::Blue);
    (*this)[7, 4].unit->heal(-80);

    (*this)[6, 4].unit = Unit::createUnit("Infantry", Team::Red);
}
template <>
void Map::displayMode<ViewMode::Shopping>(sf::RenderWindow &window)
{
    drawMap(window);
    header.setMoney(game.players[game.current_active_player].money);
    header.setName(game.players[game.current_active_player].name);
    header.draw(window);

    shopping_menu.draw(window);
}

template <>
void Map::displayMode<ViewMode::Watch>(sf::RenderWindow &window)
{
    drawMap(window);
}
void Map::resize()
{
    pos_x = (WindowManager::window_width - scale * 16 * width) / 2;
    pos_y = (WindowManager::window_height - scale * 16 * height) / 2;
}

template <>
void Map::displayMode<ViewMode::View>(sf::RenderWindow &window)
{
    drawMap(window);
    moveCursor();
    handleEvents();

    if (selected_unit)
        getTile(*selected_unit).unit->displayPath(window, *this);

    drawCursor(window);

    header.setMoney(game.players[game.current_active_player].money);
    header.setName(game.players[game.current_active_player].name);
    header.draw(window);

    unit_detail.setInfo((*this)[cursor]);
    unit_detail.draw(window);
}
template <>
void Map::displayMode<ViewMode::SelectTarget>(sf::RenderWindow &window)
{
    drawMap(window);
    moveCursor();
    drawCursor(window);

    if (WindowManager::getKeyDown(sf::Keyboard::Y))
    {
        assert(selectable_targets);
        if (selectable_targets->size() == 0 || std::find(selectable_targets->begin(), selectable_targets->end(), cursor) != selectable_targets->end())
        {
            select_function(cursor);
        }
    }
    if (WindowManager::getKeyDown(sf::Keyboard::X))
    {
        mode = SelectAction;
        clearTileEffects();
    }
}

template <>
void Map::displayMode<ViewMode::SelectAction>(sf::RenderWindow &window)
{
    drawMap(window);
    action_menu.draw(window);
}

template <>
void Map::displayMode<ViewMode::Result>(sf::RenderWindow &window)
{
    drawMap(window);
    endcard.draw(window);
}

void Map::display(sf::RenderWindow &window)
{
    if (winner != Neutral)
        displayMode<Result>(window);
    else if (game.current_active_player != team)
        displayMode<Watch>(window);
    else if (mode == View)
        displayMode<View>(window);

    else if (mode == Shopping)
        displayMode<Shopping>(window);

    // if (mode == UnitAction)
    //     displayMode<UnitAction>(window);

    else if (mode == SelectAction)
        displayMode<SelectAction>(window);

    else if (mode == SelectTarget)
        displayMode<SelectTarget>(window);
}

void Map::win(const Team &winner_team)
{
    endcard.setResult(winner_team == team, [this]()
                      { game.battle_scene.setScene("menu"); });
    winner = winner_team;
}

void Map::drawCursor(sf::RenderWindow &window)
{

    smooth_cursor_x += ((cursor.x * scale * tile_size) - smooth_cursor_x) * 10*delta_time;
    smooth_cursor_y += ((cursor.y * scale * tile_size) - smooth_cursor_y) * 10*delta_time;

    cursor_sprite.setScale(scale * tile_size / cursor_sprite.getTexture()->getSize().x, scale * tile_size / cursor_sprite.getTexture()->getSize().y);
    cursor_sprite.setPosition(smooth_x + smooth_cursor_x, smooth_y + smooth_cursor_y);
    window.draw(cursor_sprite);
}

std::vector<std::vector<unsigned>> Map::getTileIds(const std::string &data_string)
{
    std::vector<std::vector<unsigned>> tile_ids{};

    std::string_view unprocessed_data{data_string};

    MapManipulator manipulator{};
    while (true)
    {
        const std::size_t line_length = unprocessed_data.find('\n');
        const std::string_view line{unprocessed_data.data(), std::min(line_length, unprocessed_data.size())};

        auto ids = getLineData(line);
        std::ranges::transform(ids.begin(), ids.end(), ids.begin(), [&](const unsigned &x)
                               { return manipulator.getId(x); });

        tile_ids.push_back(ids);

        if (line_length == std::string::npos)
            break;

        unprocessed_data.remove_prefix(line_length + 1);
    }

    return tile_ids;
}

std::vector<std::vector<std::unique_ptr<Tile>>> Map::initTiles(const std::string &data_string)
{
    const auto tile_ids = getTileIds(data_string);
    std::vector<std::vector<std::unique_ptr<Tile>>> ret{};

    ret.reserve(tile_ids.size());
    for (auto y{0u}; y < tile_ids.size(); y++)
    {
        const auto &tile_id_row = tile_ids[y];
        std::vector<std::unique_ptr<Tile>> tile_row;
        tile_row.reserve(tile_id_row.size());
        for (auto x{0u}; x < tile_id_row.size(); x++)
            tile_row.push_back(Tile::createTile(tile_ids[y][x]));

        ret.push_back(std::move(tile_row));
    }

    return ret;
}

std::vector<unsigned> Map::getLineData(const std::string_view &line)
{
    std::vector<unsigned> line_data;
    std::string data{line};
    std::stringstream stream{data};
    std::vector<int> output;

    unsigned value;
    while (stream >> value)
    {
        line_data.push_back(value);
        stream.ignore(1);
    }

    return line_data;
}

Tile &Map::getCursorTile()
{
    return (*this)[cursor];
}

Tile &Map::getTile(const TileIndex &index)
{
    return (*this)[index];
}

void Map::handleMenu()
{

    std::map<std::string, std::function<void()>> actions{};
    std::vector<std::string> options{}; // need to check if wait or join

    options.push_back("End Turn");
    actions["End Turn"] = [this]()
    {
        game.endTurn();
        mode = View;
    };
    auto execute_action = [this, options, actions](const std::size_t &index) mutable
    {
        auto option = options[index];
        auto func = actions[option];
        func();
    };
    auto cancel_action = [this]()
    {
        mode = View;
    };

    action_menu.setOptions(options, execute_action, cancel_action);
    mode = SelectAction;
}

void Map::handleEvents()
{
    static bool show_enemy_tiles = false;
    if (WindowManager::getKeyDown(sf::Keyboard::Y))
    {
        if(show_enemy_tiles)
        {
            clearTileEffects();
            show_enemy_tiles = false;
        }

        auto &tile = getCursorTile();

        if (!selected_unit)
        {
            if (!tile.unit)
            {
                if (!tile.interact(*this, cursor))
                    handleMenu();
            }
            else if (tile.unit->getTeam() == team)
            {
                clearTileEffects();
                if (!tile.unit->select(*this, cursor))
                    handleMenu();
            }
            else
            {
                clearTileEffects();
                const auto tiles = MovementSelector::getTiles(*this, cursor, *tile.unit);
                setMovementTileMode(tiles, Tile::DisplayMode::Move);
                show_enemy_movement = true;
            }
        }
        else
        {
            assert(selected_unit);
            assert(getTile(*selected_unit).unit);
            getTile(*selected_unit).unit->act(*this, *selected_unit, cursor);
        }
    }
    if (selected_unit)
    {
        getTile(*selected_unit).unit->updateCursor(*this, cursor);
    }

    if (WindowManager::getKeyDown(sf::Keyboard::X))
    {
        clearTileEffects();
        show_enemy_tiles = false;
        if (!selected_unit && getCursorTile().unit && getCursorTile().unit->getTeam() != team)
        {
            const auto tiles = AttackableSelector::getTiles(*this, cursor, *getCursorTile().unit);
            setMovementTileMode(tiles, Tile::DisplayMode::Attack);
            show_enemy_tiles = true;
        }
        selected_unit = {};
    }
}

void Map::endTurn()
{
    for (unsigned y{0}; y < height; y++)
        for (unsigned x{0}; x < width; x++)
            if ((*this)[{x, y}].unit)
                (*this)[{x, y}].unit->setFinished(false);
}

std::shared_ptr<Unit> Map::createUnit(const std::string &id, const Team &team_id, const TileIndex &position)
{
    return createUnit(id, team_id, getTile(position));
}

std::shared_ptr<Unit> Map::createUnit(const std::string &id, const Team &team_id, Tile &tile)
{
    tile.unit = Unit::createUnit(id, team_id);
    game.players[team_id].money -= Unit::unit_costs[id];
    return tile.unit;
}

Tile &Map::operator[](const unsigned &x, const unsigned &y)
{
    return *tiles[y][x];
}

Tile &Map::operator[](const TileIndex &index)
{
    return *tiles[index.y][index.x];
}

void Map::moveMapToContain(const TileIndex &index)
{
    const auto [x, y] = getUnsmoothScreenPosition(index);

    if (x < 0)
        pos_x -= x;
    if (y < 0)
        pos_y -= y;
    if (x > WindowManager::window_width - 16 * scale)
        pos_x += WindowManager::window_width - 16 * scale - x;
    if (y > WindowManager::window_height - 16 * scale)
        pos_y += WindowManager::window_height - 16 * scale - y;
}

void Map::moveCursor()
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

    moveMapToContain(cursor);
}

bool Map::hasTeamNoUnitsLeft(const Team &team)
{
    for (unsigned y{0}; y < tiles.size(); y++)
        for (unsigned x{0}; x < tiles[y].size(); x++)
        {
            const auto &tile = (*this)[{x, y}];
            if (tile.unit && tile.unit->getTeam() == team)
                return false;
        }

    return true;
}

void Map::killUnit(const TileIndex &position)
{
    const auto team = (*this)[position].unit->getTeam();
    (*this)[position].unit = nullptr;
    if (hasTeamNoUnitsLeft(team))
        win(team == Red ? Blue : Red);

    animations.emplace_back(Animation{UI::GIF("misc/explosion/", "resources/images/"), 0.81, position, 4, 8});
}

void Map::clearTileEffects()
{
    for (unsigned y{0}; y < tiles.size(); y++)
        for (unsigned x{0}; x < tiles[y].size(); x++)
            (*this)[{x, y}].setDisplayMode(Tile::DisplayMode::Normal);
}

void Map::setMovementTileMode(const std::vector<TileIndex> &indices, const Tile::DisplayMode &mode)
{
    for (const auto &index : indices)
        getTile(index).setDisplayMode(mode);
}

std::tuple<float, float> Map::getScreenPosition(const TileIndex &index) const
{
    return {index.x * 16 * scale + smooth_x, index.y * 16 * scale + smooth_y};
}

std::tuple<float, float> Map::getUnsmoothScreenPosition(const TileIndex &index) const
{
    return {index.x * 16 * scale + pos_x, index.y * 16 * scale + pos_y};
}

void Map::moveCursorInDirection(unsigned &cursor_var, const int delta, float &time_since_movement, const unsigned &max_value)
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

void Map::drawMap(sf::RenderWindow &window)
{
    smooth_x += (pos_x - smooth_x) * 10*delta_time;
    smooth_y += (pos_y - smooth_y) * 10*delta_time;

    for (unsigned y{0}; y < height; y++)
        for (unsigned x{0}; x < width; x++)
            (*this)[{x, y}].display(window, *this, {x, y});

    for (unsigned y{0}; y < height; y++)
        for (unsigned x{0}; x < width; x++)
        {
            auto unit = (*this)[{x, y}].unit; // create copy of shared pointer if unit is killed while displaying
            if(unit)
                unit->display(window, *this, {x, y});
        }

    drawAnimations(window);
}

void Map::beginTurn(const Team &begin_turn_team)
{
    (void)begin_turn_team;
    for (unsigned y{0}; y < height; y++)
        for (unsigned x{0}; x < width; x++)
        {
            auto &tile = (*this)[{x, y}];
            tile.startRound(*this, {x, y});

            if (tile.unit)
                tile.unit->startRound(*this, {x, y});
        }
}

std::vector<TileIndex> Map::getNeighbors(const TileIndex &index)
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

void Map::drawAnimations(sf::RenderWindow &window)
{
    if (animations.empty())
        return;
    auto it = animations.begin();

    while (it != animations.end())
    {
        auto next = it;
        next++;
        if (!it->draw(window, *this))
            animations.erase(it);

        it = next;
    }
}

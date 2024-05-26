#include <game/map.hpp>
#include <game/game.hpp>
#include <scenes/battle.hpp>

Map::Map(Game &game, const std::string &data_string)
    : game{game}, cursor_sprite{gif_resources.get("unit_select.gif")},
      tiles{initTiles(data_string)}, width{(unsigned)tiles[0].size()}, height{(unsigned)tiles.size()},
      money_text{"", font_resources.get("arial.ttf")}
{
    beginTurn(team);
    (*this)[0, 0].unit = Unit::createUnit(ANTIAIR, Team::Blue);
    (*this)[0, 1].unit = Unit::createUnit(APC, Team::Blue);
    (*this)[0, 2].unit = Unit::createUnit(ARTILLERY, Team::Blue);
    (*this)[0, 3].unit = Unit::createUnit(BATTLECOPTER, Team::Blue);
    (*this)[0, 4].unit = Unit::createUnit(BATTLESHIP, Team::Blue);
    // (*this)[1, 0].unit = Unit::createUnit(BLACKBOAT, Team::Blue);
    (*this)[1, 1].unit = Unit::createUnit(BOMBER, Team::Blue);
    // (*this)[1, 2].unit = Unit::createUnit(CARRIER, Team::Blue);
    (*this)[1, 3].unit = Unit::createUnit(CRUISER, Team::Blue);
    (*this)[1, 4].unit = Unit::createUnit(FIGHTER, Team::Blue);
    (*this)[2, 0].unit = Unit::createUnit(INFANTRY, Team::Blue);
    (*this)[2, 1].unit = Unit::createUnit(LANDER, Team::Blue);
    (*this)[2, 2].unit = Unit::createUnit(MEDIUMTANK, Team::Blue);
    (*this)[2, 3].unit = Unit::createUnit(MECH, Team::Blue);
    (*this)[3, 0].unit = Unit::createUnit(MEGATANK, Team::Blue);
    (*this)[3, 1].unit = Unit::createUnit(MISSILE, Team::Blue);
    (*this)[3, 2].unit = Unit::createUnit(NEOTANK, Team::Blue);
    (*this)[3, 3].unit = Unit::createUnit(PIPERUNNER, Team::Blue);
    (*this)[3, 4].unit = Unit::createUnit(RECON, Team::Blue);
    (*this)[4, 0].unit = Unit::createUnit(ROCKET, Team::Blue);
    // (*this)[4, 1].unit = Unit::createUnit(STEALTH, Team::Blue);
    // (*this)[4, 2].unit = Unit::createUnit(SUB, Team::Blue);
    (*this)[4, 3].unit = Unit::createUnit(TCOPTER, Team::Blue);
    (*this)[4, 4].unit = Unit::createUnit(TANK, Team::Blue);
    // (*this)[4, 4].unit = Unit::createUnit("Infantry", Team::Blue);
    // (*this)[5, 4].unit = Unit::createUnit("TransportCopter", Team::Red);
    // (*this)[6, 4].unit = Unit::createUnit("Infantry", Team::Red);
    // (*this)[6, 4].unit->heal(-80);

    // (*this)[7, 4].unit = Unit::createUnit("Infantry", Team::Red);
    // (*this)[7, 4].unit->heal(-80);
}
template <>
void Map::displayMode<ViewMode::Shopping>(sf::RenderWindow &window)
{
    shopping_menu.draw(window);
}

template <>
void Map::displayMode<ViewMode::Watch>(sf::RenderWindow &window)
{
    drawMap(window);
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
    money_text.setString(std::to_string(game.players[team].money));
    money_text.setOrigin(0, 0);
    window.draw(money_text);

    if((*this)[cursor].unit)
    {
        unit_detail.setInfo((*this)[cursor].unit->health, (*this)[cursor].unit->ammo);
        unit_detail.draw(window);
    }
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
    cursor_sprite.setScale(scale * tile_size / cursor_sprite.getTexture()->getSize().x, scale * tile_size / cursor_sprite.getTexture()->getSize().y);
    cursor_sprite.setPosition(cursor.x * scale * tile_size, cursor.y * scale * tile_size);
    window.draw(cursor_sprite);
}

std::vector<std::vector<unsigned>> Map::getTileIds(const std::string &data_string)
{
    std::vector<std::vector<unsigned>> tile_ids{};

    std::string_view unprocessed_data{data_string};

    while (true)
    {
        const std::size_t line_length = unprocessed_data.find('\n');
        const std::string_view line{unprocessed_data.data(), std::min(line_length, unprocessed_data.size())};

        tile_ids.push_back(getLineData(line));

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
    if (WindowManager::getKeyDown(sf::Keyboard::Y))
    {
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
                if (!tile.unit->select(*this, cursor))
                    handleMenu();
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
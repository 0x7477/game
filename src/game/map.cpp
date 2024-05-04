#include <game/map.hpp>

template <>
void Map::displayMode<ViewMode::Shopping>(sf::RenderWindow &window)
{
    shopping_menu.draw(window);
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

void Map::display(sf::RenderWindow &window)
{
    if (mode == View)
        displayMode<View>(window);

    if (mode == Shopping)
        displayMode<Shopping>(window);

    // if (mode == UnitAction)
    //     displayMode<UnitAction>(window);

    if (mode == SelectAction)
        displayMode<SelectAction>(window);

    if (mode == SelectTarget)
        displayMode<SelectTarget>(window);
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

std::vector<std::vector<Tile>> Map::initTiles(const std::string &data_string)
{
    const auto tile_ids = getTileIds(data_string);
    std::vector<std::vector<Tile>> ret{};

    ret.reserve(tile_ids.size());
    for (auto y{0u}; y < tile_ids.size(); y++)
    {
        const auto &tile_id_row = tile_ids[y];
        std::vector<Tile> tile_row;
        tile_row.reserve(tile_id_row.size());
        for (auto x{0u}; x < tile_id_row.size(); x++)
            tile_row.push_back(Tile::createTile(tile_ids[y][x]));

        ret.push_back(tile_row);
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
        endTurn();
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
                if(!tile.interact(cursor, *this))
                    handleMenu();
            }
            else if (tile.unit->getTeam() == team)
            {
                if(!tile.unit->select(*this, cursor))
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

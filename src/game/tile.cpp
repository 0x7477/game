#include <game/tile.hpp>
#include <game/map.hpp>
#include <game/game.hpp>
#include <game/player.hpp>

void Tile::display(sf::RenderWindow &window, Map &map, const TileIndex &index)
{
    const auto [x, y] = map.getScreenPosition(index);

    // sprite.setPosition(x, y);
    // sprite.setScale(map.scale, map.scale);

    // sprite.setOrigin(0, sprite.getTexture()->getSize().y - 16);

    // sprite.setColor(display_mode == Move ? sf::Color::Blue : (display_mode == Attack ? sf::Color::Red : sf::Color::White));

    sprite.display(window, x, y - map.scale * (sprite.getTexture()->getSize().y - 16), map.scale);

    if (display_mode == Move)
        movement_effect.display(window, x, y, map.scale);
    if (display_mode == Attack)
        attack_effect.display(window, x, y, map.scale);

    // window.draw();
}

void RepairUnits::handleStartOfTurn(Map &map, const TileIndex &tile_index)
{
    if (map[tile_index].team != map.game.current_active_player)
        return;

    auto &money = map.game.players[map[tile_index].team].money;
    money += 1000;

    if (!map[tile_index].unit)
        return;

    auto &unit = *map[tile_index].unit;

    if (unit.getTeam() != map[tile_index].team)
        return;

    unit.refuel();
    const auto unit_costs = unit.getCosts();

    if (unit.getUnitCount() == 10)
        return; // dont need healing

    else if (unit.getUnitCount() == 9 && money >= unit_costs / 10)
    {
        unit.heal(map, tile_index, 10);
        money -= unit_costs / 10; // just needs small healing
    }
    else if (money >= unit_costs / 5)
    {
        unit.heal(map, tile_index, 20);
        money -= unit_costs / 5; // needs big healing
    }
    else if (money >= unit_costs / 10)
    {
        unit.heal(map, tile_index, 10);
        money -= unit_costs / 10; // can just afford small healing
    }
}

ProduceUnit::ProduceUnit(const std::vector<std::string> &factory_produced_units)
    : factory_produced_units{factory_produced_units} {}

bool ProduceUnit::interact(Map &map, const TileIndex &tile_index) const
{
    const auto &tile = map[tile_index];
    if (map.team != tile.team)
        return false;

    map.shopping_menu.setOptions(map.game.players[map.team].money, factory_produced_units, [&](const std::size_t &index)
                                 {
            map.game.sendCreateUnit(factory_produced_units[index], map.team, tile_index);
            const auto unit = map.createUnit(factory_produced_units[index], map.team, tile_index);
            unit->setFinished();
            map.mode = ViewMode::View; }, [&]()
                                 { map.mode = ViewMode::View; });
    map.mode = ViewMode::Shopping;

    return true;
}

std::optional<int> Tile::getHealth() const
{
    return {};
}

int Tile::getPotentialDamageFrom(const Unit &) const
{
    return 0;
}

std::unique_ptr<Tile> Tile::createTile(unsigned id)
{
    if (!library.contains(id))
        std::cout << "missing id " << id << "\n";
    assert(library.contains(id));
    return library[id]();
}

void Tile::setTeam(const Team &new_team)
{
    team = new_team;
    sprite = UI::GIF("tiles/" + id + "/" + std::to_string(team) + "/", "resources/images/");
}

void Tile::startRound(Map &map, const TileIndex &index)
{
    info.tile_round_behaviour.handleStartOfTurn(map, index);
}

std::string Tile::getId() const
{
    return id;
}

unsigned Tile::getMovementCost(const MovementType &type)
{
    return info.movement_costs.getCosts(type);
}

unsigned Tile::getDefense() const
{
    return info.defense;
}


void Tile::setTexture(const std::string &name)
{
    sprite = UI::GIF("tiles/" + id + "/" + name + "/", "resources/images/");
}

void Tile::setDirection(const Direction &direction_)
{
    direction = direction_;
    sprite = UI::GIF("tiles/" + id + "/" + std::to_string(direction) + "/", "resources/images/");
}

bool Tile::isAttackable(Map &map, const TileIndex &tile, const Unit &unit)
{
    return info.tile_attack_interaction.isAttackable(map, tile, unit);
}

void Tile::attack(Map &map, const TileIndex &unit, const TileIndex &tile)
{
    return info.tile_attack_interaction.attack(map, unit, tile);
}

bool Tile::interact(Map &map, const TileIndex &tile)
{
    return info.player_interaction.interact(map, tile);
}

void Tile::setDisplayMode(const TileDisplayMode &new_display_mode)
{
    display_mode = new_display_mode;
}
Tile::Tile(const TileInfo &info, const std::source_location &location)
    : id{getClassName(location)}, info{info}, sprite{"tiles/" + id + "/" + std::to_string(team) + "/", "resources/images/"},
      movement_effect{"misc/movement/", "resources/images/"},
      attack_effect{"misc/attack_tile/", "resources/images/"}
{
}

std::string Tile::getClassName(const std::source_location &location)
{
    const std::string function_name{location.function_name()};
    const auto start{function_name.find_last_of(':') + 1};
    const auto end{function_name.find_last_of(']')};

    if (end == std::string::npos)
        return function_name.substr(7, function_name.find(':', 8) - 7);

    return function_name.substr(start, end - start);
}


void Tile::updateBasedByNeighbors(std::array<Tile*, 8>&)
{}


void Tile::encodeAdditionalInfo(YAML::Node&) const
{

}

void Tile::decodeAdditionalInfo(const YAML::Node&)
{}

void Tile::encode(YAML::Node& node, const TileIndex& index) const
{
    YAML::Node info;

    info["chords"]["x"] = index.x;
    info["chords"]["y"] = index.y;
    info["team"] = (int)team;
    encodeAdditionalInfo(info);

    if(unit)
    {
        info["unit"]["id"]=unit->id;
        info["unit"]["health"]=unit->getHealth();
        info["unit"]["team"]=(int)unit->getTeam();
        info["unit"]["finished"]=unit->status.finished;
        info["unit"]["ammo"]=unit->ammo;
    }

    node.push_back(info);
}
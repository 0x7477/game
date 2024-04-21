#include <game/tile.hpp>
#include <game/map.hpp>

const std::vector<std::string> factory_produced_units{"Infantry", "TransportCopter"};
struct ProduceUnit : public TilePlayerInteraction
{
    virtual bool interact(Tile &tile, Map &map) const override
    {
        if (map.team != tile.team)
            return false;

        map.shopping_menu.setOptions(factory_produced_units, [&](const std::size_t &index)
                                     {
            const auto unit = map.createUnit(factory_produced_units[index], map.team, tile);
            unit->setFinished();
            map.mode = ViewMode::View; }, [&]()
                                     { map.mode = ViewMode::View; });
        map.mode = ViewMode::Shopping;

        return true;
    }
};

static ProduceUnit produce_unit{};

namespace Tiles
{
    struct Base : Tile
    {
        Base() : Tile(TileInfo{.defense = 3, .movement_costs = {1, 1, 1, 1, 0, 1, 1, 0},.player_interaction=produce_unit})
        {
        }
    };

    // template <>
    // struct Base<Neutral> : public Tile
    // {
    //     Base() : Tile(35, ) {}
    // };
    // template <>
    // struct Base<Red> : public Tile
    // {
    //     Base() : Tile(39, 3, {1, 1, 1, 1, 0, 1, 1, 0},produce_unit)
    //     {
    //         team = Red;
    //     }
    // };

    // template <>
    // struct Base<Blue> : public Tile
    // {
    //     Base() : Tile(44, 3, {1, 1, 1, 1, 0, 1, 1, 0},produce_unit)
    //     {
    //         team = Blue;
    //     }
    // };
}

namespace
{
    bool init = init_tile<Tiles::Base>(35) && 
                init_tile<Tiles::Base>(39,Red) && 
                init_tile<Tiles::Base>(44, Blue);
}

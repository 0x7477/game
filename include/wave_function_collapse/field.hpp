#pragma once
#include "tile.hpp"
#include "state.hpp"
#include "rule.hpp"
#include <array>
#include <algorithm>
#include <limits>
#include <map>
#include <list>

namespace wfc
{
template <unsigned int X, unsigned int Y>
class Field
{
public:
    Field(const std::vector<Rule> &rules)
        : rules{rules}
    {
        State initialState{rules};

        for (auto &row : states)
            std::fill(row.begin(), row.end(), initialState);

        generate();
    };

    std::tuple<unsigned int, unsigned int> setFirstTileRandomly()
    {
        const auto x{std::rand() % X}, y{std::rand() % Y};

        const auto random_tile_index = 1 + std::rand() % (magic_enum::enum_count<TileType>() - 1);
        const TileType random_tile = magic_enum::enum_value<TileType>(random_tile_index);

        setState(x, y, random_tile);
        return {x, y};
    }

    void regenerateLowestEntropyVector()
    {
        lowest_entropy = maximum_number_of_states;
        for (unsigned int x{0}; x < X; x++)
            for (unsigned int y{0}; y < Y; y++)
                updateEntropyVector(x, y, states[x][y]->getEntropy());
    }

    void generate()
    {
        auto [x, y] = setFirstTileRandomly();

        while (unknown_states > 0)
        {
            std::cout << "unknown_states " << unknown_states << "\n";
            const auto new_coordinates = iterate(x, y);
            x = std::get<0>(new_coordinates);
            y = std::get<1>(new_coordinates);
        }
    }

    void updateEntropyVector(const unsigned int &x, const unsigned int &y, const entropy_t &entropy)
    {
        if (entropy > lowest_entropy)
            return;

        if (entropy < lowest_entropy)
        {
            lowest_entropy = entropy;
            lowest_entropies.clear();
        }

        lowest_entropies.push_back({x, y});
    }

    void setState(const unsigned int &x, const unsigned int &y, const Tile &tile)
    {
        std::erase_if(lowest_entropies, [&](const std::tuple<unsigned int, unsigned int> &coords)
                      {
            // std::cout << std::get<0>(coords) << " " << std::get<1>(coords) << "\n";
            return std::get<0>(coords) == x && std::get<1>(coords) == y; });

        std::cout << "set " << x << " " << y << " to " << tile << "\n";
        assert(tiles[x][y] == Unknown);
        tiles[x][y] = tile;
        unknown_states--;
    }

    std::tuple<unsigned int, unsigned int> iterate(const unsigned int &x, const unsigned int &y)
    {
        for (auto rule_index = 0u; rule_index < rules.size(); rule_index++)
        {
            const auto &rule = rules[rule_index];
            if (!rule.isDirectionValid(x, y, X, Y))
                continue;
        
            if (!(rule.me == tiles[x][y]))
                continue;

            const auto tile_x{x + rule.direction.x}, tile_y{y + rule.direction.y};
            if(tiles[tile_x][tile_y] != Unknown)
                continue;

            State *updated_state = &states[tile_x][tile_y];
            updated_state->enable(rule_index);

            // std::cout << "enabled rule " << tile_x << " " << tile_y << " " << rules[rule_index].target << "\n";

            updateEntropyVector(tile_x, tile_y, updated_state->getEntropy());
        }

        // std::cout << "lowest_entropy " << (int)lowest_entropy << "\n";

        // std::cout << "lowest_entropies.size() " << lowest_entropies.size() << "\n";

        const auto collapsing_state_index = std::rand() % lowest_entropies.size();
        const auto [collapsing_state_x, collapsing_state_y] = lowest_entropies[collapsing_state_index];
        auto &collapsing_state = states[collapsing_state_x][collapsing_state_y];
        const auto rule_indices = collapsing_state.getEnabledRules();

        // std::cout << "rule_indices.size()" << rule_indices.size() << "\n";
        const auto collapsing_rule_index = std::rand() % rule_indices.size();
        const auto &collapsing_rule = rules[rule_indices[collapsing_rule_index]];

        setState(collapsing_state_x, collapsing_state_y, collapsing_rule.target);
        return {collapsing_state_x, collapsing_state_y};
    }

    void applyRules()
    {
    }

    void applyRule(const Rule &rule, const unsigned int &index)
    {
    }

    std::array<Tile, Y> &operator[](const unsigned int &x)
    {
        return tiles[x];
    }

    unsigned int unknown_states{X * Y};
    entropy_t lowest_entropy{maximum_number_of_states};
    std::vector<Rule> rules;
    std::vector<std::tuple<unsigned int, unsigned int>> lowest_entropies;
    std::array<std::array<entropy_t, Y>, X> entropies{};
    std::array<std::array<int, Y>, X> entropy_map_index{};
    std::array<std::array<State, Y>, X> states;
    std::array<std::array<Tile, Y>, X> tiles{};
};
}
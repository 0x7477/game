#pragma once
#include "pattern.hpp"
#include "tile_image.hpp"

#include "wave.hpp"
namespace wfc
{
    class RuleGenerator
    {
    public:
        RuleGenerator(const std::string &path, const std::size_t &kernel_size)
            : image(path), kernel_size{kernel_size}
        {
            image.draw();
            generatePatterns();
        }

        void addPattern(const unsigned int &x, const unsigned int &y)
        {
            const Pattern pattern(image, kernel_size, x, y, Pattern::Configuration{Pattern::DEG0, Pattern::None});
            // pattern.draw();
            
            const auto hash = pattern.getHash();

            if (patterns.contains(hash))
            {
                pattern_frequency[hash]++;
                return;
            }

            pattern_frequency[hash] = 1;
            patterns[hash] = pattern;
        }

        void generatePatterns()
        {
            for (auto x{0u}; x <= image.width - kernel_size; x++)
                for (auto y{0u}; y <= image.height - kernel_size; y++)
                    addPattern(x, y);

            std::cout << "found " << patterns.size() << " unique patterns\n";
            checkPatternAdjacency();
        }

        bool checkIfPatternsOverlap(const Pattern &pattern1, const Pattern &pattern2, const int &dx, const int &dy)
        {
            const int x_min{std::max(0, dx)}, x_max = std::min(kernel_size, kernel_size + dx);
            const int y_min{std::max(0, dy)}, y_max = std::min(kernel_size, kernel_size + dy);

            for (auto y{y_min}; y < y_max; y++)
            {
                for (auto x{x_min}; x < x_max; x++)
                {
                    const auto x1 = x;
                    const auto y1 = y - dy;
                    const auto x2 = x - dx;
                    const auto y2 = y;
                    if (pattern1[x1, y1] != pattern2[x2, y2])
                        return false;
                }
            }

            return true;
        }

        void checkPatternAdjacency()
        {
            const std::vector<std::tuple<int, int>> directions{{0, 1}, {1, 0}, {0, -1}, {-1, 0}};

            std::size_t num_of_adjacency_rules{0};
            for (const auto &[_, source_pattern] : patterns)
            {
                for (const auto &[_, neighbour_pattern] : patterns)
                {
                    for (std::size_t direction{0}; direction < 4; direction++)
                    {
                        const auto [dx, dy] = directions[direction];
                        const auto patterns_overlap{checkIfPatternsOverlap(source_pattern, neighbour_pattern, dx, dy)};
                        if (!patterns_overlap)
                            continue;

                        adjacency_rules[direction][source_pattern.getHash()].push_back(neighbour_pattern.getHash());

                        num_of_adjacency_rules++;
                    }
                }
            }

            std::cout << "found " << num_of_adjacency_rules << " adjacency rules\n";
        }

        HyperState createInitState()
        {
            return {patterns.size()};
        }

        void generateImage(const unsigned int &width, const unsigned int &height)
        {
            std::vector<PatternInfo> infos;

            const auto num_of_patterns = (image.width - kernel_size) * (image.height - kernel_size);

            std::size_t pattern_id{0};
            for (auto &[hash, pattern] : patterns)
            {
                const auto probability{(float)pattern_frequency[hash] / num_of_patterns};
                infos.push_back(PatternInfo{&pattern, pattern_id++, hash, probability, {}});
            }

            for (auto &info : infos)
            {
                std::array<std::vector<PatternInfo *>, 4> impossible_adjacent_patterns;

                for (auto direction{0u}; direction < 4; direction++)
                {
                    for (auto &neighbour : infos)
                    {
                        bool is_adjacency_possible = false;
                        
                        for (const auto adjacent_patterns : adjacency_rules[direction][info.hash])
                        {
                            if (neighbour.hash != adjacent_patterns)
                                continue;
                            
                            is_adjacency_possible = true;
                            break;
                        }

                        if(!is_adjacency_possible)
                            info.impossible_adjacent_patterns[direction].push_back(&neighbour);

                    }
                }
            }

            Wave wave(width, height, createInitState(), infos);
            const auto success = wave.run();
            std::cout << "success: " << success << "\n";
        }

        std::array<std::map<unsigned int, std::vector<unsigned int>>, 4> adjacency_rules;
        std::map<unsigned int, Pattern> patterns{};
        std::map<unsigned int, std::size_t> pattern_frequency{};
        TileImage image;
        std::size_t kernel_size;
    };
}
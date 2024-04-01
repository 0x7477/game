#pragma once
#include "pattern.hpp"
#include "tile_image.hpp"

#include "wave.hpp"
#include "direction.hpp"
namespace wfc
{
    class RuleGenerator
    {
    public:
        RuleGenerator(const std::string &path, const std::size_t &kernel_size)
            : image(path), kernel_size{kernel_size}
        {
            // image.draw();
            generatePatterns();
            checkPatternAdjacency();
        }

        void addPattern(const unsigned int &x, const unsigned int &y)
        {
            const Pattern pattern(image, kernel_size, x, y, Pattern::Configuration{Pattern::DEG0, Pattern::None});

            const auto hash = pattern.getHash();

            if (!pattern_indexes.contains(hash))
            { // we found a new pattern!

                const auto index = pattern_indexes.size();
                pattern_indexes[hash] = index;

                patterns.push_back(pattern);
                pattern_frequency.push_back(1);
                return;
            }
            // this pattern already exists

            const auto index = pattern_indexes[hash];
            pattern_frequency[index]++;
        }

        void generatePatterns()
        {
            for (auto y{0u}; y <= image.height - kernel_size; y++)
                for (auto x{0u}; x <= image.width - kernel_size; x++)
                    addPattern(x, y);

            std::cout << "found " << patterns.size() << " unique patterns\n";
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
            std::size_t num_of_adjacency_rules{0};

            for (std::size_t source_pattern_id{0u}; source_pattern_id < patterns.size(); source_pattern_id++)
            {
                adjacencies.push_back({});

                for (std::size_t direction{0}; direction < 4; direction++)
                {
                    for (std::size_t neighbour_pattern_id{0u}; neighbour_pattern_id < patterns.size(); neighbour_pattern_id++)
                    {
                        const auto [dx, dy] = directions[direction];
                        const auto patterns_overlap{checkIfPatternsOverlap(patterns[source_pattern_id], patterns[neighbour_pattern_id], dx, -dy)};
                        if (!patterns_overlap)
                            continue;

                        adjacencies[source_pattern_id][direction].push_back(neighbour_pattern_id);
                        num_of_adjacency_rules++;
                    }
                }
            }

            std::cout << "found " << num_of_adjacency_rules << " adjacency rules\n";
        }

        void generateImage(const unsigned int &width, const unsigned int &height)
        {

            const std::size_t num_of_patterns = (image.width + 1 - kernel_size) * (image.height + 1 - kernel_size);
            
            std::vector<float> pattern_probabilities(num_of_patterns);
            for (std::size_t pattern{0u}; pattern < patterns.size(); pattern++)
                pattern_probabilities.push_back((float)pattern_frequency[pattern] / num_of_patterns);
            
            std::vector<PatternInfo> infos;


            for (std::size_t pattern{0u}; pattern < patterns.size(); pattern++)
            {
                const auto probability{(float)pattern_frequency[pattern] / num_of_patterns};
                infos.push_back(PatternInfo{NULL, 0, probability, adjacencies[pattern]});
            }

            const std::size_t seed{0};

            Wave wave(width - kernel_size + 1, height - kernel_size + 1, kernel_size, infos, seed);
            const auto success = wave.run();

            for(auto y{0u}; y <= height- kernel_size; y++ )
            {
                for(auto x{0u}; x <= width- kernel_size; x++ )
                    std::cout << (int)patterns[wave[x,y]][0,0] << " ";
                
                for(auto x{1u}; x < kernel_size; x++ )
                    std::cout << (int)patterns[wave[width-1- kernel_size,y]][x,0] << " ";

                std::cout << "\n";
            }
            for(auto y{1u}; y < kernel_size; y++ )
            {
                for(auto x{0u}; x <= width- kernel_size; x++ )
                    std::cout << (int)patterns[wave[x,height- kernel_size]][0,y] << " ";
                
                for(auto x{1u}; x < kernel_size; x++ )
                    std::cout << (int)patterns[wave[width-1- kernel_size,height- kernel_size]][x,y] << " ";

                std::cout << "\n";
            }


            std::cout << "success: " << success << "\n";
        }

        std::map<unsigned int, std::size_t> pattern_indexes{};
        std::vector<Pattern> patterns{};
        std::vector<std::size_t> pattern_frequency{};
        std::vector<std::array<std::vector<std::size_t>, 4>> adjacencies{};
        TileImage image;
        std::size_t kernel_size;
    };
}
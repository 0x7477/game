#pragma once
#include <string>
#include <png.h>
#include <fstream>
#include <iostream>
#include <memory>
#include <set>

#include "rule.hpp"

namespace wfc
{
class RuleGenerator
{
public:
    RuleGenerator(const std::string &path)
    {
        readImage(path);
    }


    unsigned int getTile(const unsigned int& x,const  unsigned int& y)
    {
        return pixel[x + y* width];
    }

    Rule generateRule(const unsigned int& x1, const unsigned int& y1,const unsigned int& x2,const  unsigned int& y2)
    {
        return Rule{Tile{getTile(x1,y1)}, Tile{getTile(x2,y2)}, Direction{(int8_t)(x2-x1), (int8_t)(y2-y1)}};
    }

    std::vector<Rule> generateRules(const int &kernel_size)
    {
        std::set<Rule> rules{};

        for (unsigned int y = 0; y < height; y++)
        {
            for (unsigned int x = 0; x < width; x++)
            {
                for (int i = -kernel_size; i <= kernel_size; i++)
                {
                    for (int j = -kernel_size; j <= kernel_size; j++)
                    {
                        if(!i && !j) continue;

                        if(x+i < 0 || x+i >= width) continue;
                        if(y+j < 0 || y+j >= height) continue;

                        rules.insert(generateRule(x,y,x+i,y+j));
                        rules.insert(generateRule(x+i,y+j,x,y));
                    }
                }
            }
        }
        for(const auto& rule: rules)
            std::cout << rule << "\n";

        return std::vector<Rule>{rules.begin(), rules.end()};
    }

    void readImage(const std::string &path)
    {
        auto file_deleter = [](FILE *file)
        { if (file) fclose(file); };
        const std::unique_ptr<FILE, decltype(file_deleter)> image(std::fopen(path.c_str(), "rb"), file_deleter);

        png_structp png_ptr{png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr)};
        png_infop info_ptr = png_create_info_struct(png_ptr);
        png_init_io(png_ptr, image.get());
        png_read_info(png_ptr, info_ptr);

        width = png_get_image_width(png_ptr, info_ptr);
        height = png_get_image_height(png_ptr, info_ptr);

        pixel.reserve(width * height);

        png_bytep *row_pointers = new png_bytep[height];
        for (unsigned int y = 0; y < height; y++)
            row_pointers[y] = new png_byte[png_get_rowbytes(png_ptr, info_ptr)];

        png_read_image(png_ptr, row_pointers);

        for (unsigned int y = 0; y < height; y++)
        {
            for (unsigned int x = 0; x < width; x++)
            {
                png_bytep px = &(row_pointers[y][x * 4]);

                const std::tuple rgb{px[0], px[1], px[2]};

                if (!colors.contains(rgb))
                    colors[rgb] = ++max_tile;

                pixel[x + y * width] = colors[rgb];
                std::cout << pixel[x + y * width] << " ";
            }
            std::cout << std::endl;
        }

        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);

        for (unsigned int y = 0; y < height; y++)
            delete[] row_pointers[y];
        delete[] row_pointers;
    }

    unsigned int max_tile{0};
    unsigned int width, height;
    std::vector<unsigned int> pixel;
    std::map<std::tuple<png_byte, png_byte, png_byte>,unsigned int> colors;
};
}
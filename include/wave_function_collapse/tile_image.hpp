#pragma once
#include <png.h>
#include <fstream>
#include <iostream>
#include <memory>
#include <set>
#include <vector>
#include <map>

#include "tile.hpp"

namespace wfc
{

class TileImage
{
public:
    TileImage(const std::string &path)
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
        png_byte color_type = png_get_color_type(png_ptr, info_ptr);
        png_byte bit_depth = png_get_bit_depth(png_ptr, info_ptr);

        pixel.reserve(width * height);

        png_bytep *row_pointers = new png_bytep[height];
        for (unsigned int y = 0; y < height; y++)
            row_pointers[y] = new png_byte[png_get_rowbytes(png_ptr, info_ptr)];

        png_read_image(png_ptr, row_pointers);

        std::cout << "Image width: " << width << std::endl;
        std::cout << "Image height: " << height << std::endl;
        std::cout << "Color type: " << static_cast<int>(color_type) << std::endl;
        std::cout << "Bit depth: " << static_cast<int>(bit_depth) << std::endl;

        std::map<std::tuple<png_byte, png_byte, png_byte>, unsigned int> colors;

        for (unsigned int y = 0; y < height; y++)
        {
            for (unsigned int x = 0; x < width; x++)
            {

                if (color_type == PNG_COLOR_TYPE_RGBA)
                {

                    png_bytep px = &(row_pointers[y][x * 4]);

                    const std::tuple rgb{px[0], px[1], px[2]};

                    if (!colors.contains(rgb))
                        colors[rgb] = maximum_tile_id++;

                    pixel[x + y * width] = colors[rgb];
                } else if (color_type == PNG_COLOR_TYPE_RGB)
                {
                    png_bytep px = &(row_pointers[y][x * 3]);
                    const std::tuple rgb{px[0], px[1], px[2]};

                    if (!colors.contains(rgb))
                        colors[rgb] = maximum_tile_id++;

                    pixel[x + y * width] = colors[rgb];

                }
            }
        }

        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);

        for (unsigned int y = 0; y < height; y++)
            delete[] row_pointers[y];
        delete[] row_pointers;
    }

    void draw() const
    {
        for (unsigned int y = 0; y < height; y++)
        {
            for (unsigned int x = 0; x < width; x++)
                std::cout << (int)pixel[x + y * width] << " ";

            std::cout << std::endl;
        }
    }

    TileId &operator[](std::size_t x, std::size_t y)
    {
        return pixel[x + y * width];
    }

    TileId operator[](std::size_t x, std::size_t y) const
    {
        return pixel[x + y * width];
    }

    TileId maximum_tile_id{0};
    unsigned int width, height;
    std::vector<TileId> pixel;
};
}
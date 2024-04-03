#pragma once
#include <string>
#include <sstream>
#include <span>
#include <vector>
#include <SFML/Graphics.hpp>
#include "resource_loader.hpp"

class Map
{
    public:
    Map(const std::string& data_strong)
    {
        std::string_view unprocessed_data{data_strong};

        while(true)
        {
            const std::size_t line_length = unprocessed_data.find('\n');
            const std::string_view line{unprocessed_data.data(), std::min(line_length,unprocessed_data.size())};

            tiles.push_back(getLineData(line));

            if(line_length == std::string::npos)
                break;

            unprocessed_data.remove_prefix(line_length+1);
        }

        width = tiles[0].size();
        height = tiles.size();

    }

    std::vector<unsigned> getLineData(const std::string_view& line)
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

    void display(sf::RenderWindow &window)
    {
        sf::Sprite tile;
        

           
        for(unsigned y{0}; y < tiles.size(); y++)
        {
            for(unsigned x{0}; x < tiles[y].size(); x++)
            {
                const auto tile_id = tiles[y][x];
                tile.setTexture(image_resources.get(std::to_string(tile_id) + ".gif"));
                const auto texture_size = tile.getTexture()->getSize();
                tile.setOrigin(0,texture_size.y);
                tile.setPosition(x*16,y*16);
                tile.setTextureRect(sf::IntRect(0, 0, texture_size.x, texture_size.y));


                window.draw(tile);
            }
        }
    }

    std::size_t width, height;
    std::vector<std::vector<unsigned>> tiles{};
};
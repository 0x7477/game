#pragma once
#include <string>
#include <filesystem>
#include <SFML/Graphics.hpp>
#include <any>
#include <iostream>

template<typename T>
class ResourceLoader
{
    public:
    ResourceLoader(const std::string& path, const std::string& file_extension)
    {
        const std::filesystem::path resources_folder ("resources/");

        root_path = (resources_folder/path).string();
        for(const auto& file : std::filesystem::recursive_directory_iterator(resources_folder/path))
        {
            if(file.is_directory()) 
                continue;
            if(file.path().filename().extension().string() != file_extension)
                continue;

            T resource;

            resource.loadFromFile(file.path().string());
            resources[file.path().lexically_relative(resources_folder/path).string()] = resource;
        }
    }
    

    T& get(const std::string& name)
    {
        if(!resources.contains(name))
        {
            std::cerr << "unknown resource: " << name << "\n";
            return resources["missing.png"];
        }
        
        return resources[name];
    }

    
    std::string root_path;
    std::map<std::string, T> resources{};

    
};

inline ResourceLoader<sf::Font> font_resources{"fonts", ".ttf"};
inline ResourceLoader<sf::Texture> image_resources{"images", ".png"};
inline ResourceLoader<sf::Texture> gif_resources{"images", ".gif"};
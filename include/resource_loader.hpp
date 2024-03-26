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
    ResourceLoader(const std::string& path)
    {
        const std::filesystem::path resources_folder ("resources/");
        for(const auto& file : std::filesystem::recursive_directory_iterator(resources_folder/path))
        {
            if(file.is_directory()) 
                continue;
            T resource;

            resource.loadFromFile(file.path().string());
            resources[file.path().filename().string()] = resource;
        }
    }
    

    static T& get(const std::string& name)
    {
        if(!resources.contains(name))
        {
            std::cerr << "unknown resource: " << name << "\n";
            return resources["missing.png"];
        }
        
        return resources[name];
    }

    static inline std::map<std::string, T> resources;

    
};

template<>
std::map<std::string, sf::Font> ResourceLoader<sf::Font>::resources{};
template<>
std::map<std::string, sf::Texture> ResourceLoader<sf::Texture>::resources{};


inline ResourceLoader<sf::Font> font_resources{"fonts"};
inline ResourceLoader<sf::Texture> image_resources{"images"};
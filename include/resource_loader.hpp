#pragma once
#include <string>
#include <filesystem>
#include <SFML/Graphics.hpp>
#include <any>

template<typename T>
class ResourceLoader
{
    public:
    ResourceLoader(const std::string& path)
    {
        const std::filesystem::path resources_folder ("resources/");
        for(const auto& file : std::filesystem::directory_iterator(resources_folder/path))
        {
            T resource;
            resource.loadFromFile(file.path().string());
            resources[file.path().filename().string()] = resource;
        }
    }
    

    static T& get(const std::string& name)
    {
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
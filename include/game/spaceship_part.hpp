#pragma once
#include <SFML/Graphics.hpp>
#include <map>
#include <source_location>
#include <iostream>
#include <resource_loader.hpp>

class SpaceshipPart
{
    static inline std::map<std::string, SpaceshipPart*> parts;

protected:
    SpaceshipPart()
    {
    }

public:

    static const sf::Texture* getPartTexture(const std::string& part)
    {
        if(!parts.contains(part))
            return &image_resources.get("missing.png");
        
        return parts[part]->getTexture();
    }


    static SpaceshipPart* getPart(const std::string& part)
    {
        if(!parts.contains(part))
            return nullptr;
        return parts[part];
    }
    void draw(sf::RenderWindow& window)
    {
        window.draw(sprite);
    }

    template<typename T>
    static void registerClass()
    {
        const auto class_name = getClassName();
        const auto instance = new T();
        instance->init(class_name);
        parts[class_name] = instance;
    }

    const sf::Texture* getTexture()
    {
        const auto texture = sprite.getTexture();
        if(!texture)
            return &ResourceLoader<sf::Texture>::get("missing.png");
        return texture;
    } 

    private:

    void init(const std::string& name)
    {
        sprite.setTexture(image_resources.get(name + ".png"));
    }

    static std::string getClassName(const std::source_location& location = std::source_location::current()) 
    {
        const std::string function_name{location.function_name()};
        const auto start{function_name.find_last_of(':')+1};
        const auto end{function_name.find_last_of(']')};
        return function_name.substr(start,end-start);
    }

    virtual void control(){};

    sf::Sprite sprite;
    int health;
};
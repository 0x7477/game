#pragma once
#include <vector>
#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <any>
#include <typeindex>
#include <memory>


class GameObject
{
public:
    GameObject(){}

    template<typename T>
    T& getComponent()
    {
        return *std::any_cast<std::shared_ptr<T>>(components[typeid(T)]);
    }

    template<typename T, typename... Args>
    void addComponent(const Args&... args )
    {
        components[typeid(T)] = std::make_shared<T>(this, args...);
    }

    std::unordered_map<std::type_index, std::any> components;

    virtual void update(){};
    virtual void display(sf::RenderWindow &){};


    
};


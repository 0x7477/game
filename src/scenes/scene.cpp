#include <engine/scene.hpp>
#include <engine/window_manager.hpp>
#include <iostream>


Scene::Scene::Scene(const std::string& name, WindowManager &window_manager)
    :window_manager{window_manager}
{
    scenes[name] = this;
}

void Scene::Scene::setScene(const std::string& name)
{
    if(!scenes.contains(name))
        std::cerr << "unknown scene: " << name << "\n";
    window_manager.scene = scenes[name];
}

void Scene::Scene::run() {}

#pragma once
#include "window_manager.hpp"
#include <map>

namespace Scene
{
    class Scene
    {
    protected:
        Scene(const std::string& name, WindowManager &window_manager);

    public:
        virtual void run();

        void setScene(const std::string& name);
        virtual void resize();

    protected:
        WindowManager &window_manager;
    };

    inline std::map<std::string, Scene*> scenes;
}
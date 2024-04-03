// #pragma once

// #include <engine/window_manager.hpp>
// #include <engine/scene.hpp>
// #include <SFML/Graphics.hpp>
// #include <helper/delta_timer.hpp>
// #include <cmath>
// #include <ui/panel.hpp>
// #include <ui/parts_menu.hpp>
// #include <map>
// #include <tuple>
// #include <spaceship_part.hpp>
// namespace Scene
// {

//     class Editor : public Scene
//     {

//     public:
//         Editor(WindowManager &window_manager);

//         void scrollView();
//         void drawGrid();
//         void drawUI();
//         virtual void run() override;

//         std::tuple<float, float> convertScreenToScreenGridPosition(const std::tuple<float, float> &mouse_position);
//         std::tuple<int, int> convertScreenToGridPosition(const std::tuple<float, float> &mouse_position);
//         std::tuple<float, float> convertGridToScreenPosition(const std::tuple<int, int> &grid_position);

//         void selectSpaceshipPart(const std::string &part);
//         bool checkIfAllPartsAreConnected();

//     private:
//         float scroll_level{50};
//         float view_x{10}, view_y{10};

//         int mouse_x{0}, mouse_y{0};

//         std::map<std::tuple<int, int>, SpaceshipPart *> parts{};
//         UI::Panel panel{{0, 0, 0, 80_percent}};
//         UI::PartsMenu parts_menu{{0, 20_percent, 0, 0}, [this](const std::string &part)
//                                  { selectSpaceshipPart(part); }};
//         sf::Texture hexagon;
//         sf::Sprite selected_part_view;
//         SpaceshipPart *selected_spaceship_part{nullptr};
//         DeltaTimer time_delta{};
//     };
// }

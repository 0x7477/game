// #include <scenes/editor.hpp>
// #include <iostream>
// #include "resource_loader.hpp"
// #include <cmath>
// #include <spaceship_part.hpp>
// Scene::Editor::Editor(WindowManager &window_manager)
//     : Scene{"editor", window_manager},
//       hexagon{image_resources.get("hexagon.png")}
// {

//     panel.setColor(sf::Color::White);
//     selected_part_view.setPosition(10, 10);
// }

// std::tuple<float, float> Scene::Editor::convertScreenToScreenGridPosition(const std::tuple<float, float> &mouse_position)
// {
//     const auto [cursor_x, cursor_y] = mouse_position;

//     const auto view_offset_x = fmodf(view_x, scroll_level);
//     const auto view_offset_y = fmodf(view_y, scroll_level * 0.75);

//     auto y_index = int((cursor_y - view_offset_y) / (scroll_level * 0.75));
//     bool even_row = (int)(view_y / (scroll_level * .75)) % 2;
//     bool x_index_offset{0};
//     const bool upper_part = fmodf(cursor_y - view_offset_y, (scroll_level * 0.75)) < scroll_level * 0.25;

//     if (upper_part)
//     {
//         const float x = fmodf(cursor_x - view_offset_x - (y_index % 2 + (int)even_row) * scroll_level / 2, scroll_level) / scroll_level;
//         const float y = 1 - fmodf(cursor_y - view_offset_y, scroll_level * 0.75) / (scroll_level * 0.25);

//         const bool above = y >= -2 * std::abs(x - 0.5) + 1;
//         if (above)
//         {
//             y_index--;
//             if (x > 0.5)
//                 x_index_offset = 1;
//         }
//     }

//     const auto x_pos = cursor_x - fmodf(cursor_x - view_offset_x + (y_index % 2 + (int)even_row) * scroll_level * 0.5 + (int)x_index_offset * scroll_level, scroll_level);
//     const auto y_pos = y_index * scroll_level * 0.75 + view_offset_y;

//     return {x_pos, y_pos};
// }

// std::tuple<int, int> Scene::Editor::convertScreenToGridPosition(const std::tuple<float, float> &mouse_position)
// {
//     const auto [x_pos, y_pos] = convertScreenToScreenGridPosition(mouse_position);
//     return {floor((x_pos - view_x) / scroll_level), floor((y_pos - view_y) / (scroll_level * 0.75))};
// }

// std::tuple<float, float> Scene::Editor::convertGridToScreenPosition(const std::tuple<int, int> &grid_position)
// {
//     const auto [grid_x, grid_y] = grid_position;

//     const auto pos_y = grid_y * scroll_level * 0.75;
//     const auto pos_x = (grid_x + 0.5 * (grid_y % 2)) * scroll_level;

//     return {pos_x + view_x, pos_y + view_y};
// }

// bool GridPositionsAreNeighbours(const std::tuple<int, int> pos1, std::tuple<int, int> pos2)
// {
//     const auto [pos1_g_x, pos1_g_y] = pos1;
//     const auto [pos2_g_x, pos2_g_y] = pos2;

//     const auto pos1_x = (pos1_g_x + 0.5 * (pos1_g_y % 2));
//     const auto pos1_y = pos1_g_y * 0.75;

//     const auto pos2_x = (pos2_g_x + 0.5 * (pos2_g_y % 2));
//     const auto pos2_y = pos2_g_y * 0.75;

//     const auto dx = pos2_x - pos1_x;
//     const auto dy = pos2_y - pos1_y;

//     return sqrt(dx * dx + dy * dy) <= 1;
// }

// bool Scene::Editor::checkIfAllPartsAreConnected()
// {
//     if (parts.size() <= 1)
//         return true;
//     std::vector<std::tuple<std::tuple<int, int>, bool>> part_positions;
//     part_positions.reserve(parts.size());

//     for (const auto &[index, _] : parts)
//         part_positions.push_back({index, false});

//     std::get<bool>(part_positions[0]) = true;

//     std::size_t missing_parts = parts.size() - 1;

//     while (true)
//     {
//         std::size_t connected_tiles{0};
//         for (auto &[index, is_connected] : part_positions)
//         {
//             if (is_connected)
//                 continue;

//             for (const auto &[potential_neighbour, is_potential_neighbour_connected] : part_positions)
//             {
//                 if (!is_potential_neighbour_connected)
//                     continue;
//                 if (!GridPositionsAreNeighbours(index, potential_neighbour))
//                     continue;

//                 std::cout << "connected\n";

//                 connected_tiles++;
//                 missing_parts--;
//                 if (missing_parts == 0)
//                     return true;

//                 is_connected = true;
//                 break;
//             }
//         }

//         if (connected_tiles == 0)
//             return false;
//     }
// }

// void Scene::Editor::drawGrid()
// {
//     auto &window = window_manager.window;
//     sf::Sprite hex_sprite{hexagon};
//     hex_sprite.setColor(sf::Color::Black);
//     sf::Sprite cursor{hexagon};

//     hex_sprite.setScale(scroll_level / hexagon.getSize().x, scroll_level / hexagon.getSize().y);
//     cursor.setScale(scroll_level / hexagon.getSize().x, scroll_level / hexagon.getSize().y);

//     float offset = scroll_level / 2;

//     for (auto y{-2}; y - 2 < (window.getSize().y / scroll_level) / 0.75; y++)
//     {
//         offset = offset == 0 ? scroll_level / 2 : 0;
//         for (auto x{-2}; x - 2 < window.getSize().x / scroll_level; x++)
//         {
//             hex_sprite.setPosition(fmodf(view_x, scroll_level) + x * scroll_level + offset, (1.5) * fmodf(view_y / 1.5, scroll_level) + (0.75) * y * scroll_level);
//             window.draw(hex_sprite);
//         }
//     }

//     const auto cursor_x = sf::Mouse::getPosition(window).x;
//     const auto cursor_y = sf::Mouse::getPosition(window).y;

//     sf::Sprite part_sprite;
//     for (const auto &[index, part] : parts)
//     {
//         const auto [ship_part_x, ship_part_y] = convertGridToScreenPosition(index);

//         const auto texture = part->getTexture();
//         part_sprite.setTexture(*texture);
//         part_sprite.setPosition(ship_part_x, ship_part_y);
//         part_sprite.setScale(scroll_level / texture->getSize().x, scroll_level / texture->getSize().y);

//         window.draw(part_sprite);
//     }

//     const auto [x_pos, y_pos] = convertScreenToScreenGridPosition({cursor_x, cursor_y});
//     cursor.setPosition(x_pos, y_pos);
//     window.draw(cursor);

//     const auto cursor_grid_position = convertScreenToGridPosition({cursor_x, cursor_y});
//     mouse_x = std::get<0>(cursor_grid_position);
//     mouse_y = std::get<1>(cursor_grid_position);
// }

// void Scene::Editor::drawUI()
// {
//     window_manager.window.draw(selected_part_view);
//     panel.draw(window_manager.window);
//     parts_menu.draw(window_manager.window);
// }

// void Scene::Editor::selectSpaceshipPart(const std::string &part)
// {
//     const auto part_pointer = SpaceshipPart::getPart(part);

//     if (!part_pointer)
//         return;

//     const float width{100}, height{100};
//     selected_spaceship_part = part_pointer;
//     const auto texture = selected_spaceship_part->getTexture();
//     const auto texture_width{texture->getSize().x}, texture_height{texture->getSize().y};
//     selected_part_view.setScale(width / texture_width, height / texture_height);
//     selected_part_view.setTexture(*texture);
//     selected_part_view.setTextureRect(sf::IntRect(0, 0, texture_width, texture_height));
// }

// void Scene::Editor::scrollView()
// {
//     const auto mouse_pos = sf::Mouse::getPosition(window_manager.window);
//     static bool was_pressed{false};
//     static int last_mouse_pos_x{0}, last_mouse_pos_y{0};

//     float scroll_delta{0};

//     if (WindowManager::keys[sf::Keyboard::Up])
//         scroll_delta += time_delta;
//     if (WindowManager::keys[sf::Keyboard::Down])
//         scroll_delta -= time_delta;

//     const float scroll_speed = 100;

//     scroll_delta += WindowManager::mouse_delta * time_delta * scroll_speed;

//     scroll_delta *= scroll_level * 0.1;

//     const auto center_x = (-view_x + WindowManager::window_width / 2) / scroll_level;
//     const auto center_y = (-view_y + WindowManager::window_height / 2) / scroll_level;

//     view_x -= center_x * scroll_delta;
//     view_y -= center_y * scroll_delta;

//     scroll_level += scroll_delta;
//     scroll_level = std::clamp(scroll_level, 10.f, 100.f);

//     if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
//     {
//         if (was_pressed)
//         {
//             view_x += (mouse_pos.x - last_mouse_pos_x);
//             view_y += (mouse_pos.y - last_mouse_pos_y);
//         }
//         else if (selected_spaceship_part)
//         {
//             parts[{mouse_x, mouse_y}] = selected_spaceship_part;

//             std::cout << checkIfAllPartsAreConnected() << "\n";
//         }

//         last_mouse_pos_x = mouse_pos.x;
//         last_mouse_pos_y = mouse_pos.y;
//     }

//     was_pressed = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);
// }

// void Scene::Editor::run()
// {
//     auto &window = window_manager.window;
//     time_delta.update();
//     scrollView();

//     window.clear(sf::Color(35, 45, 55));

//     drawGrid();

//     drawUI();
//     window.display();
// }

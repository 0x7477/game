cmake_minimum_required(VERSION 3.25.2)
project(game)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_FLAGS "-Wall -Werror -Wpedantic -O3 -march=native -g")

file(GLOB_RECURSE src src/**.cpp)
# file(GLOB_RECURSE spaceship_parts src/spaceship_parts/**.cpp)

list(FILTER src EXCLUDE REGEX "server_main.cpp$")
list(FILTER src EXCLUDE REGEX "client_main.cpp$")
list(FILTER src EXCLUDE REGEX "generator_main.cpp$")
# list(FILTER src EXCLUDE REGEX ".*src/spaceship_parts/.*")

# add_library(${PROJECT_NAME}_lib ${src})

add_executable(${PROJECT_NAME} src/generator_main.cpp)

message(${src})

add_executable(server ${src} src/server_main.cpp)
add_executable(client ${src} src/client_main.cpp)


# target_link_libraries(${PROJECT_NAME}_lib png uv)
target_link_libraries(${PROJECT_NAME} png uv sfml-system sfml-window sfml-graphics sfml-audio X11)
target_link_libraries(server png uv sfml-system sfml-window sfml-graphics sfml-audio X11)
target_link_libraries(client png uv sfml-system sfml-window sfml-graphics sfml-audio X11)

include_directories(include)

# Copy resources directory to build directory
file(COPY resources DESTINATION ${CMAKE_BINARY_DIR})

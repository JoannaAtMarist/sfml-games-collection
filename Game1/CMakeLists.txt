# Game1 CMakeLists.txt
cmake_minimum_required(VERSION 3.16)
project(Game1)

set(CMAKE_CXX_STANDARD 17)

find_package(SFML 2.5 COMPONENTS graphics window system audio REQUIRED)

file(GLOB SOURCES "*.cpp")

add_executable(Game1 Game1.cpp)

target_link_libraries(Game1
    sfml-graphics
    sfml-window
    sfml-system
    sfml-audio
)

add_custom_command(TARGET Game1 POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_CURRENT_SOURCE_DIR}/graphics
            $<TARGET_FILE_DIR:Game1>/graphics
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_CURRENT_SOURCE_DIR}/sound
            $<TARGET_FILE_DIR:Game1>/sound
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_CURRENT_SOURCE_DIR}/fonts
            $<TARGET_FILE_DIR:Game1>/fonts
)

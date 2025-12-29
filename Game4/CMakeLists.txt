# Game4 CMakeLists.txt
cmake_minimum_required(VERSION 3.16)
project(Game4)

set(CMAKE_CXX_STANDARD 17)

find_package(SFML 2.5 COMPONENTS graphics window system audio REQUIRED)

file(GLOB SOURCES "*.cpp")

add_executable(Game4 ${SOURCES})

target_link_libraries(Game4
    sfml-graphics
    sfml-window
    sfml-system
    sfml-audio
)

add_custom_command(TARGET Game4 POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_CURRENT_SOURCE_DIR}/graphics
            $<TARGET_FILE_DIR:Game4>/graphics
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_CURRENT_SOURCE_DIR}/music
            $<TARGET_FILE_DIR:Game4>/music
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_CURRENT_SOURCE_DIR}/fonts
            $<TARGET_FILE_DIR:Game4>/fonts
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_CURRENT_SOURCE_DIR}/sound
            $<TARGET_FILE_DIR:Game4>/sound
)


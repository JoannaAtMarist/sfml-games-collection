# Game3 CMakeLists.txt
cmake_minimum_required(VERSION 3.16)
project(Game3)

set(CMAKE_CXX_STANDARD 17)

find_package(SFML 2.5 COMPONENTS graphics window system audio REQUIRED)

file(GLOB SOURCES "*.cpp")

add_executable(Game3 ${SOURCES})

target_link_libraries(Game3
    sfml-graphics
    sfml-window
    sfml-system
    sfml-audio
)

add_custom_command(TARGET Game3 POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_CURRENT_SOURCE_DIR}/fonts
            $<TARGET_FILE_DIR:Game3>/fonts
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_CURRENT_SOURCE_DIR}/gamedata
            $<TARGET_FILE_DIR:Game3>/gamedata
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_CURRENT_SOURCE_DIR}/graphics
            $<TARGET_FILE_DIR:Game3>/graphics
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_CURRENT_SOURCE_DIR}/sound
            $<TARGET_FILE_DIR:Game3>/sound
)


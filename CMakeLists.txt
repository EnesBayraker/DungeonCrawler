cmake_minimum_required(VERSION 3.28)

project(DungeonCrawler LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include(FetchContent)

FetchContent_Declare(
        SFML
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        GIT_TAG 3.1.0
        GIT_SHALLOW ON
        EXCLUDE_FROM_ALL
        SYSTEM
)

FetchContent_MakeAvailable(SFML)

add_executable(DungeonCrawler
        src/main.cpp
        src/Player.cpp
        src/Map.cpp
        src/Entity.h
        src/Entity.cpp
        src/Enemy.h
        src/Enemy.cpp
        src/MessageLog.h
        src/MessageLog.cpp
        src/GameUI.h
        src/GameUI.cpp
        src/Item.h
        src/Item.cpp
        src/SaveSystem.h
        src/SaveSystem.cpp
        src/Game.h
        src/Game.cpp
)

target_compile_features(DungeonCrawler PRIVATE cxx_std_17)

target_link_libraries(DungeonCrawler PRIVATE
        SFML::Graphics
)

add_custom_command(TARGET DungeonCrawler POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/assets
        $<TARGET_FILE_DIR:DungeonCrawler>/assets
)

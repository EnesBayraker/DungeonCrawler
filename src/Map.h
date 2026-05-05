#pragma once

#include <SFML/Graphics.hpp>
#include <random>
#include <vector>

enum class TileType
{
    Floor,
    Wall
};

struct Room
{
    int x;
    int y;
    int width;
    int height;

    sf::Vector2i getCenter() const;
};

class Map
{
public:
    static constexpr int TileSize = 32;
    static constexpr int Width = 25;
    static constexpr int Height = 18;

    Map();

    void draw(sf::RenderWindow& window) const;
    bool isWalkable(int x, int y) const;
    sf::Vector2i getPlayerStart() const;

private:
    struct Area
    {
        int x;
        int y;
        int width;
        int height;
    };

    std::vector<std::vector<TileType>> m_tiles;
    std::vector<Room> m_rooms;
    sf::Vector2i m_playerStart;
    std::mt19937 m_randomEngine;

    void generateBspMap();
    void splitArea(const Area& area, int depth);
    void createRoomInArea(const Area& area);
    void carveRoom(const Room& room);

    int randomInt(int min, int max);
};
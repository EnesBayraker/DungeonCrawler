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

    void generateNewFloor();

    void draw(sf::RenderWindow& window) const;
    bool isWalkable(int x, int y) const;
    bool isVisible(int x, int y) const;
    bool canSee(const sf::Vector2i& origin, const sf::Vector2i& target, int radius) const;
    TileType getTile(int x, int y) const;
    bool isExplored(int x, int y) const;

    void computeFov(const sf::Vector2i& origin, int radius);

    sf::Vector2i getPlayerStart() const;
    sf::Vector2i getStairsPosition() const;
    const std::vector<Room>& getRooms() const;

private:
    struct Area
    {
        int x;
        int y;
        int width;
        int height;
    };

    std::vector<std::vector<TileType>> m_tiles;
    std::vector<std::vector<bool>> m_visible;
    std::vector<std::vector<bool>> m_explored;
    std::vector<Room> m_rooms;

    sf::Vector2i m_playerStart;
    sf::Vector2i m_stairsPosition;
    std::mt19937 m_randomEngine;

    void generateBspMap();
    void splitArea(const Area& area, int depth);
    void createRoomInArea(const Area& area);
    void carveRoom(const Room& room);
    void placeStairs();
    void connectRooms();
    void carveHorizontalCorridor(int x1, int x2, int y);
    void carveVerticalCorridor(int y1, int y2, int x);
    void setFloor(int x, int y);

    bool isInsideMap(int x, int y) const;
    bool isTransparent(int x, int y) const;
    bool hasLineOfSight(const sf::Vector2i& origin, const sf::Vector2i& target) const;

    int randomInt(int min, int max);
};
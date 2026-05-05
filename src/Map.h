#pragma once

#include <SFML/Graphics.hpp>
#include <vector>

enum class TileType
{
    Floor,
    Wall
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

private:
    std::vector<std::vector<TileType>> m_tiles;

    void createTestMap();
};
#include "Map.h"

Map::Map()
{
    createTestMap();
}

void Map::createTestMap()
{
    m_tiles.resize(Height, std::vector<TileType>(Width, TileType::Floor));

    // Haritanın dış sınırlarını duvar yapıyorum
    for (int y = 0; y < Height; ++y)
    {
        for (int x = 0; x < Width; ++x)
        {
            if (x == 0 || y == 0 || x == Width - 1 || y == Height - 1)
            {
                m_tiles[y][x] = TileType::Wall;
            }
        }
    }

    // Test için içeriye birkaç sabit duvar ekliyorum
    for (int x = 5; x < 15; ++x)
    {
        m_tiles[6][x] = TileType::Wall;
    }

    for (int y = 9; y < 15; ++y)
    {
        m_tiles[y][18] = TileType::Wall;
    }
}

bool Map::isWalkable(int x, int y) const
{
    if (x < 0 || x >= Width || y < 0 || y >= Height)
    {
        return false;
    }

    return m_tiles[y][x] == TileType::Floor;
}

void Map::draw(sf::RenderWindow& window) const
{
    sf::RectangleShape tileShape(
        sf::Vector2f(static_cast<float>(TileSize), static_cast<float>(TileSize))
    );

    for (int y = 0; y < Height; ++y)
    {
        for (int x = 0; x < Width; ++x)
        {
            tileShape.setPosition({
                static_cast<float>(x * TileSize),
                static_cast<float>(y * TileSize)
            });

            if (m_tiles[y][x] == TileType::Wall)
            {
                tileShape.setFillColor(sf::Color(100, 100, 100));
            }
            else
            {
                tileShape.setFillColor(sf::Color(35, 35, 35));
            }

            window.draw(tileShape);
        }
    }
}
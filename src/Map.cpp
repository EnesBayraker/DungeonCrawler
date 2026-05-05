#include "Map.h"


sf::Vector2i Room::getCenter() const
{
    return {
        x + width / 2,
        y + height / 2
    };
}

Map::Map()
    : m_playerStart(1, 1),
      m_randomEngine(std::random_device{}())
{
    generateBspMap();
}

void Map::generateBspMap()
{
    // Haritayı başta tamamen duvar yapıyoruz.
    m_tiles.assign(Height, std::vector<TileType>(Width, TileType::Wall));
    m_rooms.clear();

    // Dış sınırları korumak için BSP alanını bir tile içeriden başlatıyoruz.
    Area rootArea{
        1,
        1,
        Width - 2,
        Height - 2
    };

    splitArea(rootArea, 0);

    if (!m_rooms.empty())
    {
        // Şimdilik oyuncuyu ilk odanın merkezine koyacağız.
        // Rastgele oda spawn mantığı Gün 5'te gelecek.
        m_playerStart = m_rooms.front().getCenter();
    }
}

void Map::splitArea(const Area& area, int depth)
{
    constexpr int MaxDepth = 4;
    constexpr int MinLeafSize = 6;

    const bool canSplitHorizontally = area.height >= MinLeafSize * 2;
    const bool canSplitVertically = area.width >= MinLeafSize * 2;

    if (depth >= MaxDepth || (!canSplitHorizontally && !canSplitVertically))
    {
        createRoomInArea(area);
        return;
    }

    bool splitHorizontally = false;

    if (canSplitHorizontally && canSplitVertically)
    {
        splitHorizontally = randomInt(0, 1) == 0;
    }
    else
    {
        splitHorizontally = canSplitHorizontally;
    }

    if (splitHorizontally)
    {
        const int split = randomInt(MinLeafSize, area.height - MinLeafSize);

        Area topArea{
            area.x,
            area.y,
            area.width,
            split
        };

        Area bottomArea{
            area.x,
            area.y + split,
            area.width,
            area.height - split
        };

        splitArea(topArea, depth + 1);
        splitArea(bottomArea, depth + 1);
    }
    else
    {
        const int split = randomInt(MinLeafSize, area.width - MinLeafSize);

        Area leftArea{
            area.x,
            area.y,
            split,
            area.height
        };

        Area rightArea{
            area.x + split,
            area.y,
            area.width - split,
            area.height
        };

        splitArea(leftArea, depth + 1);
        splitArea(rightArea, depth + 1);
    }
}

void Map::createRoomInArea(const Area& area)
{
    constexpr int RoomPadding = 1;
    constexpr int MinRoomSize = 3;

    const int maxRoomWidth = area.width - RoomPadding * 2;
    const int maxRoomHeight = area.height - RoomPadding * 2;

    if (maxRoomWidth < MinRoomSize || maxRoomHeight < MinRoomSize)
    {
        return;
    }

    const int roomWidth = randomInt(MinRoomSize, maxRoomWidth);
    const int roomHeight = randomInt(MinRoomSize, maxRoomHeight);

    const int roomX = area.x + randomInt(RoomPadding, area.width - roomWidth - RoomPadding);
    const int roomY = area.y + randomInt(RoomPadding, area.height - roomHeight - RoomPadding);

    Room room{
        roomX,
        roomY,
        roomWidth,
        roomHeight
    };

    m_rooms.push_back(room);
    carveRoom(room);
}

void Map::carveRoom(const Room& room)
{
    // Oda alanındaki duvar tile'larını zemine çeviriyoruz.
    for (int y = room.y; y < room.y + room.height; ++y)
    {
        for (int x = room.x; x < room.x + room.width; ++x)
        {
            if (x >= 0 && x < Width && y >= 0 && y < Height)
            {
                m_tiles[y][x] = TileType::Floor;
            }
        }
    }
}

int Map::randomInt(int min, int max)
{
    if (min >= max)
    {
        return min;
    }

    std::uniform_int_distribution<int> distribution(min, max);
    return distribution(m_randomEngine);
}

bool Map::isWalkable(int x, int y) const
{
    if (x < 0 || x >= Width || y < 0 || y >= Height)
    {
        return false;
    }

    return m_tiles[y][x] == TileType::Floor;
}

sf::Vector2i Map::getPlayerStart() const
{
    return m_playerStart;
}

void Map::draw(sf::RenderWindow& window) const
{
    sf::RectangleShape tileShape(
        sf::Vector2f(static_cast<float>(TileSize), static_cast<float>(TileSize))
    );

    // Tile sınırlarını hafif belli etmek için ince çizgi ekliyoruz.
    tileShape.setOutlineThickness(1.f);
    tileShape.setOutlineColor(sf::Color(20, 20, 20));

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
                tileShape.setFillColor(sf::Color(95, 95, 95));
            }
            else
            {
                tileShape.setFillColor(sf::Color(55, 55, 55));
            }

            window.draw(tileShape);
        }
    }
}
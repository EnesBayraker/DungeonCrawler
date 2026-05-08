#include "Map.h"

#include <algorithm>
#include <cstdlib>

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
    m_visible.assign(Height, std::vector<bool>(Width, false));
    m_explored.assign(Height, std::vector<bool>(Width, false));
    m_rooms.clear();

    // Dış sınırları korumak için BSP alanını bir tile içeriden başlatıyoruz.
    Area rootArea{
        1,
        1,
        Width - 2,
        Height - 2
    };

    splitArea(rootArea, 0);
    connectRooms();

    if (!m_rooms.empty())
    {
        // Oyuncuyu rastgele bir odanın merkezinde başlatıyoruz.
        const int roomIndex = randomInt(0, static_cast<int>(m_rooms.size()) - 1);
        m_playerStart = m_rooms[roomIndex].getCenter();
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
            setFloor(x, y);
        }
    }
}

void Map::connectRooms()
{
    if (m_rooms.size() < 2)
    {
        return;
    }

    for (std::size_t i = 1; i < m_rooms.size(); ++i)
    {
        const sf::Vector2i previousCenter = m_rooms[i - 1].getCenter();
        const sf::Vector2i currentCenter = m_rooms[i].getCenter();

        // Koridorun yönünü rastgele seçiyoruz.
        if (randomInt(0, 1) == 0)
        {
            carveHorizontalCorridor(previousCenter.x, currentCenter.x, previousCenter.y);
            carveVerticalCorridor(previousCenter.y, currentCenter.y, currentCenter.x);
        }
        else
        {
            carveVerticalCorridor(previousCenter.y, currentCenter.y, previousCenter.x);
            carveHorizontalCorridor(previousCenter.x, currentCenter.x, currentCenter.y);
        }
    }
}

void Map::carveHorizontalCorridor(int x1, int x2, int y)
{
    const int startX = std::min(x1, x2);
    const int endX = std::max(x1, x2);

    for (int x = startX; x <= endX; ++x)
    {
        setFloor(x, y);
    }
}

void Map::carveVerticalCorridor(int y1, int y2, int x)
{
    const int startY = std::min(y1, y2);
    const int endY = std::max(y1, y2);

    for (int y = startY; y <= endY; ++y)
    {
        setFloor(x, y);
    }
}

void Map::setFloor(int x, int y)
{
    if (isInsideMap(x, y))
    {
        m_tiles[y][x] = TileType::Floor;
    }
}

bool Map::isInsideMap(int x, int y) const
{
    return x >= 0 && x < Width && y >= 0 && y < Height;
}

bool Map::isTransparent(int x, int y) const
{
    if (!isInsideMap(x, y))
    {
        return false;
    }

    return m_tiles[y][x] == TileType::Floor;
}

bool Map::hasLineOfSight(const sf::Vector2i& origin, const sf::Vector2i& target) const
{
    int x0 = origin.x;
    int y0 = origin.y;
    const int x1 = target.x;
    const int y1 = target.y;

    const int dx = std::abs(x1 - x0);
    const int sx = x0 < x1 ? 1 : -1;

    const int dy = -std::abs(y1 - y0);
    const int sy = y0 < y1 ? 1 : -1;

    int error = dx + dy;

    while (true)
    {
        if (x0 == x1 && y0 == y1)
        {
            return true;
        }

        // Başlangıç tile'ı hariç, aradaki duvarlar görüşü keser.
        if (!(x0 == origin.x && y0 == origin.y) && !isTransparent(x0, y0))
        {
            return false;
        }

        const int error2 = 2 * error;

        if (error2 >= dy)
        {
            error += dy;
            x0 += sx;
        }

        if (error2 <= dx)
        {
            error += dx;
            y0 += sy;
        }
    }
}

void Map::computeFov(const sf::Vector2i& origin, int radius)
{
    m_visible.assign(Height, std::vector<bool>(Width, false));

    const int radiusSquared = radius * radius;

    for (int y = origin.y - radius; y <= origin.y + radius; ++y)
    {
        for (int x = origin.x - radius; x <= origin.x + radius; ++x)
        {
            if (!isInsideMap(x, y))
            {
                continue;
            }

            const int dx = x - origin.x;
            const int dy = y - origin.y;

            if (dx * dx + dy * dy > radiusSquared)
            {
                continue;
            }

            if (hasLineOfSight(origin, {x, y}))
            {
                m_visible[y][x] = true;

                // Bir tile bir kere görünürse artık keşfedilmiş sayılır.
                m_explored[y][x] = true;
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
    if (!isInsideMap(x, y))
    {
        return false;
    }

    return m_tiles[y][x] == TileType::Floor;
}

bool Map::isVisible(int x, int y) const
{
    if (!isInsideMap(x, y))
    {
        return false;
    }

    return m_visible[y][x];
}

sf::Vector2i Map::getPlayerStart() const
{
    return m_playerStart;
}

const std::vector<Room>& Map::getRooms() const
{
    return m_rooms;
}

void Map::draw(sf::RenderWindow& window) const
{
    sf::RectangleShape tileShape(
        sf::Vector2f(static_cast<float>(TileSize), static_cast<float>(TileSize))
    );

    tileShape.setOutlineThickness(1.f);

    for (int y = 0; y < Height; ++y)
    {
        for (int x = 0; x < Width; ++x)
        {
            tileShape.setPosition({
                static_cast<float>(x * TileSize),
                static_cast<float>(y * TileSize)
            });

            const bool visible = m_visible[y][x];
            const bool explored = m_explored[y][x];

            if (!explored)
            {
                // Hiç görülmemiş alan tamamen karanlık.
                tileShape.setFillColor(sf::Color::Black);
                tileShape.setOutlineColor(sf::Color::Black);
            }
            else if (!visible)
            {
                // Daha önce görülmüş ama şu an görüş dışında olan alan.
                tileShape.setOutlineColor(sf::Color(10, 10, 10));

                if (m_tiles[y][x] == TileType::Wall)
                {
                    tileShape.setFillColor(sf::Color(35, 35, 35));
                }
                else
                {
                    tileShape.setFillColor(sf::Color(20, 20, 20));
                }
            }
            else
            {
                // Şu an aktif olarak görülen alan.
                tileShape.setOutlineColor(sf::Color(45, 45, 45));

                if (m_tiles[y][x] == TileType::Wall)
                {
                    tileShape.setFillColor(sf::Color(110, 110, 110));
                }
                else
                {
                    tileShape.setFillColor(sf::Color(65, 65, 65));
                }
            }

            window.draw(tileShape);
        }
    }
}
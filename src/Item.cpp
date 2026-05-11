#include "Item.h"

#include "Map.h"

namespace
{
    std::string getItemName(ItemType type)
    {
        switch (type)
        {
            case ItemType::Potion:
                return "Health Potion";
            case ItemType::Weapon:
                return "Iron Sword";
            case ItemType::Armor:
                return "Leather Armor";
        }

        return "Unknown Item";
    }

    sf::Color getItemColor(ItemType type)
    {
        switch (type)
        {
            case ItemType::Potion:
                return sf::Color(80, 220, 100);
            case ItemType::Weapon:
                return sf::Color(220, 200, 60);
            case ItemType::Armor:
                return sf::Color(80, 170, 220);
        }

        return sf::Color::White;
    }
}

Item::Item(ItemType type, const sf::Vector2i& gridPosition)
    : m_type(type),
      m_name(getItemName(type)),
      m_gridPosition(gridPosition),
      m_color(getItemColor(type))
{
}

void Item::draw(sf::RenderWindow& window) const
{
    // Itemler oyuncu ve düşmandan daha küçük
    sf::RectangleShape shape({18.f, 18.f});
    shape.setFillColor(m_color);

    shape.setPosition({
        static_cast<float>(m_gridPosition.x * Map::TileSize + 7),
        static_cast<float>(m_gridPosition.y * Map::TileSize + 7)
    });

    window.draw(shape);
}

ItemType Item::getType() const
{
    return m_type;
}

const std::string& Item::getName() const
{
    return m_name;
}

sf::Vector2i Item::getGridPosition() const
{
    return m_gridPosition;
}
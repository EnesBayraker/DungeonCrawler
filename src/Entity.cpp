#include "Entity.h"

#include "Map.h"

Entity::Entity(const sf::Vector2i& gridPosition, int hp, int damage, const sf::Color& color)
    : m_gridPosition(gridPosition),
      m_hp(hp),
      m_damage(damage)
{
    // Entity tile içinde biraz küçük çizilir.
    m_shape.setSize({28.f, 28.f});
    m_shape.setFillColor(color);

    updateShapePosition();
}

void Entity::draw(sf::RenderWindow& window) const
{
    window.draw(m_shape);
}

sf::Vector2i Entity::getGridPosition() const
{
    return m_gridPosition;
}

void Entity::setGridPosition(const sf::Vector2i& position)
{
    m_gridPosition = position;
    updateShapePosition();
}

int Entity::getHp() const
{
    return m_hp;
}

int Entity::getDamage() const
{
    return m_damage;
}

bool Entity::isAlive() const
{
    return m_hp > 0;
}

void Entity::takeDamage(int amount)
{
    m_hp -= amount;

    if (m_hp < 0)
    {
        m_hp = 0;
    }
}

void Entity::updateShapePosition()
{
    m_shape.setPosition({
        static_cast<float>(m_gridPosition.x * Map::TileSize + 2),
        static_cast<float>(m_gridPosition.y * Map::TileSize + 2)
    });
}
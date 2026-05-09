#include "Enemy.h"

#include <algorithm>
#include <array>
#include <cstdlib>

#include "Map.h"

Enemy::Enemy(const sf::Vector2i& gridPosition)
    : Entity(gridPosition, 10, 3, sf::Color::Red),
      m_state(EnemyState::Wander),
      m_randomEngine(std::random_device{}())
{
}

void Enemy::updateAI(
    const Map& map,
    const sf::Vector2i& playerPosition,
    const std::vector<sf::Vector2i>& occupiedPositions,
    bool forceChase
)
{
    const bool canSeePlayer = map.canSee(m_gridPosition, playerPosition, SightRadius);

    if (canSeePlayer || forceChase)
    {
        m_state = EnemyState::Chase;
    }
    else
    {
        m_state = EnemyState::Wander;
    }

    sf::Vector2i direction{0, 0};

    if (m_state == EnemyState::Chase)
    {
        direction = chooseChaseDirection(playerPosition);
    }
    else
    {
        direction = chooseWanderDirection();
    }

    const sf::Vector2i targetPosition = m_gridPosition + direction;

    if (canMoveTo(targetPosition, map, occupiedPositions, playerPosition))
    {
        setGridPosition(targetPosition);
    }
}

EnemyState Enemy::getState() const
{
    return m_state;
}

sf::Vector2i Enemy::chooseChaseDirection(const sf::Vector2i& playerPosition) const
{
    const int dx = playerPosition.x - m_gridPosition.x;
    const int dy = playerPosition.y - m_gridPosition.y;

    // Oyuncuya bitişikse şimdilik hareket etmiyor.
    // Gün 10'da burada saldırı mantığı devreye girecek.
    if (std::abs(dx) + std::abs(dy) <= 1)
    {
        return {0, 0};
    }

    if (std::abs(dx) > std::abs(dy))
    {
        return {dx > 0 ? 1 : -1, 0};
    }

    if (dy != 0)
    {
        return {0, dy > 0 ? 1 : -1};
    }

    return {0, 0};
}

sf::Vector2i Enemy::chooseWanderDirection()
{
    std::array<sf::Vector2i, 5> directions{
        sf::Vector2i{0, -1},
        sf::Vector2i{0, 1},
        sf::Vector2i{-1, 0},
        sf::Vector2i{1, 0},
        sf::Vector2i{0, 0}
    };

    std::shuffle(directions.begin(), directions.end(), m_randomEngine);

    return directions.front();
}

bool Enemy::canMoveTo(
    const sf::Vector2i& targetPosition,
    const Map& map,
    const std::vector<sf::Vector2i>& occupiedPositions,
    const sf::Vector2i& playerPosition
) const
{
    if (targetPosition == m_gridPosition)
    {
        return false;
    }

    if (targetPosition == playerPosition)
    {
        return false;
    }

    if (!map.isWalkable(targetPosition.x, targetPosition.y))
    {
        return false;
    }

    for (const sf::Vector2i& occupiedPosition : occupiedPositions)
    {
        if (targetPosition == occupiedPosition)
        {
            return false;
        }
    }

    return true;
}
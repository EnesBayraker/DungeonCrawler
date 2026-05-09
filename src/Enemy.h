#pragma once

#include <random>
#include <vector>

#include "Entity.h"

class Map;

enum class EnemyState
{
    Wander,
    Chase
};

class Enemy : public Entity
{
public:
    explicit Enemy(const sf::Vector2i& gridPosition);

    void updateAI(
        const Map& map,
        const sf::Vector2i& playerPosition,
        const std::vector<sf::Vector2i>& occupiedPositions,
        bool forceChase
    );

    EnemyState getState() const;

private:
    static constexpr int SightRadius = 6;

    EnemyState m_state;
    std::mt19937 m_randomEngine;

    sf::Vector2i chooseChaseDirection(const sf::Vector2i& playerPosition) const;
    sf::Vector2i chooseWanderDirection();

    bool canMoveTo(
        const sf::Vector2i& targetPosition,
        const Map& map,
        const std::vector<sf::Vector2i>& occupiedPositions,
        const sf::Vector2i& playerPosition
    ) const;
};
#pragma once

#include <random>
#include <string>
#include <vector>

#include "Entity.h"

class Map;
class MessageLog;
class Player;

enum class EnemyType
{
    Goblin,
    Skeleton,
    Orc
};

enum class EnemyState
{
    Wander,
    Chase
};

class Enemy : public Entity
{
public:
    explicit Enemy(const sf::Vector2i& gridPosition);
    Enemy(EnemyType type, const sf::Vector2i& gridPosition);

    void updateAI(
        const Map& map,
        Player& player,
        const std::vector<sf::Vector2i>& occupiedPositions,
        bool forceChase,
        MessageLog& messageLog
    );

    EnemyState getState() const;
    EnemyType getType() const;
    const std::string& getName() const;

private:
    static constexpr int SightRadius = 6;

    EnemyType m_type;
    std::string m_name;
    EnemyState m_state;
    std::mt19937 m_randomEngine;

    sf::Vector2i chooseChaseDirection(const sf::Vector2i& playerPosition) const;
    sf::Vector2i chooseWanderDirection();

    bool isAdjacentToPlayer(const sf::Vector2i& playerPosition) const;

    bool canMoveTo(
        const sf::Vector2i& targetPosition,
        const Map& map,
        const std::vector<sf::Vector2i>& occupiedPositions,
        const sf::Vector2i& playerPosition
    ) const;
};
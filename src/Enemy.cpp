#include "Enemy.h"

Enemy::Enemy(const sf::Vector2i& gridPosition)
    : Entity(gridPosition, 10, 3, sf::Color::Red)
{
}
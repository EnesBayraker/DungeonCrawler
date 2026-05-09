#include "Player.h"

#include "Enemy.h"
#include "Map.h"

Player::Player()
    : Entity({2, 2}, 30, 5, sf::Color::White)
{
}

bool Player::handleInput(const sf::Event& event, const Map& map, std::vector<Enemy>& enemies)
{
    const auto* keyPressed = event.getIf<sf::Event::KeyPressed>();

    if (keyPressed == nullptr)
    {
        return false;
    }

    using Scancode = sf::Keyboard::Scancode;

    if (keyPressed->scancode == Scancode::W || keyPressed->scancode == Scancode::Up)
    {
        return tryMoveOrAttack({0, -1}, map, enemies);
    }

    if (keyPressed->scancode == Scancode::S || keyPressed->scancode == Scancode::Down)
    {
        return tryMoveOrAttack({0, 1}, map, enemies);
    }

    if (keyPressed->scancode == Scancode::A || keyPressed->scancode == Scancode::Left)
    {
        return tryMoveOrAttack({-1, 0}, map, enemies);
    }

    if (keyPressed->scancode == Scancode::D || keyPressed->scancode == Scancode::Right)
    {
        return tryMoveOrAttack({1, 0}, map, enemies);
    }

    return false;
}

bool Player::tryMoveOrAttack(
    const sf::Vector2i& direction,
    const Map& map,
    std::vector<Enemy>& enemies
)
{
    const sf::Vector2i targetPosition = m_gridPosition + direction;

    for (Enemy& enemy : enemies)
    {
        if (enemy.isAlive() && enemy.getGridPosition() == targetPosition)
        {
            // Düşmanın tile'ına yürümek yerine saldırıyoruz.
            enemy.takeDamage(m_damage);
            return true;
        }
    }

    if (map.isWalkable(targetPosition.x, targetPosition.y))
    {
        setGridPosition(targetPosition);
        return true;
    }

    return false;
}
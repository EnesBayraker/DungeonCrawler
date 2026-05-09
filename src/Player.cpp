#include "Player.h"

#include "Map.h"

Player::Player()
    : Entity({2, 2}, 30, 5, sf::Color::White)
{
}

bool Player::handleInput(const sf::Event& event, const Map& map)
{
    const auto* keyPressed = event.getIf<sf::Event::KeyPressed>();

    if (keyPressed == nullptr)
    {
        return false;
    }

    using Scancode = sf::Keyboard::Scancode;

    if (keyPressed->scancode == Scancode::W || keyPressed->scancode == Scancode::Up)
    {
        return tryMove({0, -1}, map);
    }

    if (keyPressed->scancode == Scancode::S || keyPressed->scancode == Scancode::Down)
    {
        return tryMove({0, 1}, map);
    }

    if (keyPressed->scancode == Scancode::A || keyPressed->scancode == Scancode::Left)
    {
        return tryMove({-1, 0}, map);
    }

    if (keyPressed->scancode == Scancode::D || keyPressed->scancode == Scancode::Right)
    {
        return tryMove({1, 0}, map);
    }

    return false;
}

bool Player::tryMove(const sf::Vector2i& direction, const Map& map)
{
    const sf::Vector2i targetPosition = m_gridPosition + direction;

    // Hedef tile yürünebilirse oyuncu bir turn harcar.
    if (map.isWalkable(targetPosition.x, targetPosition.y))
    {
        setGridPosition(targetPosition);
        return true;
    }

    return false;
}
#include "Player.h"

#include "Map.h"

Player::Player()
    : Entity({2, 2}, 30, 5, sf::Color::White)
{
}

void Player::handleInput(const sf::Event& event, const Map& map)
{
    const auto* keyPressed = event.getIf<sf::Event::KeyPressed>();

    if (keyPressed == nullptr)
    {
        return;
    }

    using Scancode = sf::Keyboard::Scancode;

    if (keyPressed->scancode == Scancode::W || keyPressed->scancode == Scancode::Up)
    {
        tryMove({0, -1}, map);
    }
    else if (keyPressed->scancode == Scancode::S || keyPressed->scancode == Scancode::Down)
    {
        tryMove({0, 1}, map);
    }
    else if (keyPressed->scancode == Scancode::A || keyPressed->scancode == Scancode::Left)
    {
        tryMove({-1, 0}, map);
    }
    else if (keyPressed->scancode == Scancode::D || keyPressed->scancode == Scancode::Right)
    {
        tryMove({1, 0}, map);
    }
}

void Player::tryMove(const sf::Vector2i& direction, const Map& map)
{
    const sf::Vector2i targetPosition = m_gridPosition + direction;

    // Hedef tile yürünebilirse oyuncuyu oraya taşıyoruz.
    if (map.isWalkable(targetPosition.x, targetPosition.y))
    {
        setGridPosition(targetPosition);
    }
}
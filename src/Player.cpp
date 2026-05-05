#include "Player.h"

#include "Map.h"

Player::Player()
    : m_gridPosition(2, 2)
{
    // Oyuncu tile içinde biraz küçük çizilir, böylece zemin kenarı görünür.
    m_shape.setSize({28.f, 28.f});
    m_shape.setFillColor(sf::Color::White);

    updateShapePosition();
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

    // Hedef tile yürünebilirse oyuncuyu oraya taşıyorum
    if (map.isWalkable(targetPosition.x, targetPosition.y))
    {
        m_gridPosition = targetPosition;
        updateShapePosition();
    }
}

void Player::updateShapePosition()
{
    m_shape.setPosition({
        static_cast<float>(m_gridPosition.x * Map::TileSize + 2),
        static_cast<float>(m_gridPosition.y * Map::TileSize + 2)
    });
}

void Player::setGridPosition(const sf::Vector2i& position)
{
    m_gridPosition = position;
    updateShapePosition();
}

void Player::draw(sf::RenderWindow& window) const
{
    window.draw(m_shape);
}
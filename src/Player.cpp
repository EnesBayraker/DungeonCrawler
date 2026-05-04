#include "Player.h"

Player::Player()
    : m_speed(220.f)
{
    // Oyuncuyu beyaz bir kare olarak temsil ediyoruz.
    m_shape.setSize({32.f, 32.f});
    m_shape.setFillColor(sf::Color::White);
    m_shape.setPosition({384.f, 284.f});
}

void Player::handleInput(float deltaTime)
{
    sf::Vector2f movement{0.f, 0.f};

    // WASD ve ok tuşları ile hareket.
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up))
    {
        movement.y -= m_speed * deltaTime;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down))
    {
        movement.y += m_speed * deltaTime;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left))
    {
        movement.x -= m_speed * deltaTime;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right))
    {
        movement.x += m_speed * deltaTime;
    }

    m_shape.move(movement);
}

void Player::draw(sf::RenderWindow& window) const
{
    window.draw(m_shape);
}
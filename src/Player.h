#pragma once

#include <SFML/Graphics.hpp>

class Player
{
public:
    Player();

    void handleInput(float deltaTime);
    void draw(sf::RenderWindow& window) const;

private:
    sf::RectangleShape m_shape;
    float m_speed;
};
#pragma once

#include <SFML/Graphics.hpp>

class Map;

class Player
{
public:
    Player();

    void handleInput(const sf::Event& event, const Map& map);
    void draw(sf::RenderWindow& window) const;

    void setGridPosition(const sf::Vector2i& position);
    sf::Vector2i getGridPosition() const;

private:
    sf::RectangleShape m_shape;
    sf::Vector2i m_gridPosition;

    void tryMove(const sf::Vector2i& direction, const Map& map);
    void updateShapePosition();
};
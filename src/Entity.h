#pragma once

#include <SFML/Graphics.hpp>

class Entity
{
public:
    Entity(const sf::Vector2i& gridPosition, int hp, int damage, const sf::Color& color);
    virtual ~Entity() = default;

    virtual void draw(sf::RenderWindow& window) const;

    sf::Vector2i getGridPosition() const;
    void setGridPosition(const sf::Vector2i& position);

    int getHp() const;
    int getMaxHp() const;
    int getDamage() const;
    bool isAlive() const;

    void takeDamage(int amount);
    void heal(int amount);
    void increaseDamage(int amount);
    void increaseMaxHp(int amount);

protected:
    sf::RectangleShape m_shape;
    sf::Vector2i m_gridPosition;

    int m_hp;
    int m_maxHp;
    int m_damage;

    void updateShapePosition();
};
#pragma once

#include <SFML/Graphics.hpp>

#include <string>

enum class ItemType
{
    Potion,
    Weapon,
    Armor
};

class Item
{
public:
    Item(ItemType type, const sf::Vector2i& gridPosition);

    void draw(sf::RenderWindow& window) const;

    ItemType getType() const;
    const std::string& getName() const;
    sf::Vector2i getGridPosition() const;

private:
    ItemType m_type;
    std::string m_name;
    sf::Vector2i m_gridPosition;
    sf::Color m_color;
};
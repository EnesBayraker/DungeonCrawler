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

    sf::Vector2i getGridPosition() const;
    ItemType getType() const;
    const std::string& getName() const;

private:
    ItemType m_type;
    std::string m_name;
    sf::Vector2i m_gridPosition;
    sf::RectangleShape m_shape;

    inline static sf::Texture s_potionTexture;
    inline static sf::Texture s_weaponTexture;
    inline static sf::Texture s_armorTexture;
    inline static bool s_texturesLoaded = false;

    static bool loadSharedTextures();
    const sf::Texture* getTextureForType() const;
};
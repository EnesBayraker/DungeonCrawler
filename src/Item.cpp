#include "Item.h"

#include "Map.h"

namespace
{
    std::string getItemName(ItemType type)
    {
        switch (type)
        {
            case ItemType::Potion:
                return "Health Potion";
            case ItemType::Weapon:
                return "Sword";
            case ItemType::Armor:
                return "Armor";
        }

        return "Item";
    }

    sf::Color getItemColor(ItemType type)
    {
        switch (type)
        {
            case ItemType::Potion:
                return sf::Color(200, 50, 70);
            case ItemType::Weapon:
                return sf::Color(180, 180, 190);
            case ItemType::Armor:
                return sf::Color(90, 120, 210);
        }

        return sf::Color::White;
    }
}

bool Item::loadSharedTextures()
{
    if (s_texturesLoaded)
    {
        return true;
    }

    const bool potionLoaded = s_potionTexture.loadFromFile("assets/items/potion.png");
    const bool weaponLoaded = s_weaponTexture.loadFromFile("assets/items/weapon.png");
    const bool armorLoaded = s_armorTexture.loadFromFile("assets/items/armor.png");

    if (potionLoaded && weaponLoaded && armorLoaded)
    {
        s_potionTexture.setSmooth(false);
        s_weaponTexture.setSmooth(false);
        s_armorTexture.setSmooth(false);

        s_texturesLoaded = true;
        return true;
    }

    return false;
}

const sf::Texture* Item::getTextureForType() const
{
    if (!s_texturesLoaded)
    {
        return nullptr;
    }

    switch (m_type)
    {
        case ItemType::Potion:
            return &s_potionTexture;
        case ItemType::Weapon:
            return &s_weaponTexture;
        case ItemType::Armor:
            return &s_armorTexture;
    }

    return nullptr;
}

Item::Item(ItemType type, const sf::Vector2i& gridPosition)
    : m_type(type),
      m_name(getItemName(type)),
      m_gridPosition(gridPosition)
{
    loadSharedTextures();

    m_shape.setSize({20.f, 20.f});
    m_shape.setFillColor(getItemColor(type));
    m_shape.setPosition({
        static_cast<float>(gridPosition.x * Map::TileSize + 6),
        static_cast<float>(gridPosition.y * Map::TileSize + 6)
    });
}

void Item::draw(sf::RenderWindow& window) const
{
    const sf::Texture* texture = getTextureForType();

    if (texture != nullptr)
    {
        sf::Sprite sprite(*texture);

        sprite.setPosition({
            static_cast<float>(m_gridPosition.x * Map::TileSize),
            static_cast<float>(m_gridPosition.y * Map::TileSize)
        });

        window.draw(sprite);
        return;
    }

    // Texture yüklenemezse renkli kare fallback.
    window.draw(m_shape);
}

sf::Vector2i Item::getGridPosition() const
{
    return m_gridPosition;
}

ItemType Item::getType() const
{
    return m_type;
}

const std::string& Item::getName() const
{
    return m_name;
}
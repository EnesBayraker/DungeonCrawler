#pragma once

#include <SFML/Graphics.hpp>

#include <cstddef>
#include <vector>

#include "Entity.h"
#include "Item.h"

class Enemy;
class Map;
class MessageLog;

class Player : public Entity
{
public:
    Player();

    void draw(sf::RenderWindow& window) const override;

    bool handleInput(
        const sf::Event& event,
        const Map& map,
        std::vector<Enemy>& enemies,
        MessageLog& messageLog
    );

    void addItem(const Item& item);
    void clearInventory();
    bool useInventoryItem(std::size_t index, MessageLog& messageLog);

    const std::vector<Item>& getInventory() const;

private:
    std::vector<Item> m_inventory;

    inline static sf::Texture s_texture;
    inline static bool s_textureLoaded = false;

    static bool loadSharedTexture();

    bool tryMoveOrAttack(
        const sf::Vector2i& direction,
        const Map& map,
        std::vector<Enemy>& enemies,
        MessageLog& messageLog
    );
};
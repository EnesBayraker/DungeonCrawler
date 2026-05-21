#include "Player.h"

#include <string>

#include "Enemy.h"
#include "Map.h"
#include "MessageLog.h"

bool Player::loadSharedTexture()
{
    if (s_textureLoaded)
    {
        return true;
    }

    if (s_texture.loadFromFile("assets/sprites/player.png"))
    {
        s_texture.setSmooth(false);
        s_textureLoaded = true;
        return true;
    }

    return false;
}

Player::Player()
    : Entity({2, 2}, 30, 5, sf::Color::White)
{
    loadSharedTexture();
}

void Player::draw(sf::RenderWindow& window) const
{
    if (s_textureLoaded)
    {
        sf::Sprite sprite(s_texture);

        sprite.setPosition({
            static_cast<float>(m_gridPosition.x * Map::TileSize),
            static_cast<float>(m_gridPosition.y * Map::TileSize)
        });

        window.draw(sprite);
        return;
    }

    // Sprite yüklenemezse eski kare çizim fallback olarak kalsın.
    Entity::draw(window);
}

bool Player::handleInput(
    const sf::Event& event,
    const Map& map,
    std::vector<Enemy>& enemies,
    MessageLog& messageLog
)
{
    const auto* keyPressed = event.getIf<sf::Event::KeyPressed>();

    if (keyPressed == nullptr)
    {
        return false;
    }

    using Scancode = sf::Keyboard::Scancode;

    if (keyPressed->scancode == Scancode::W || keyPressed->scancode == Scancode::Up)
    {
        return tryMoveOrAttack({0, -1}, map, enemies, messageLog);
    }

    if (keyPressed->scancode == Scancode::S || keyPressed->scancode == Scancode::Down)
    {
        return tryMoveOrAttack({0, 1}, map, enemies, messageLog);
    }

    if (keyPressed->scancode == Scancode::A || keyPressed->scancode == Scancode::Left)
    {
        return tryMoveOrAttack({-1, 0}, map, enemies, messageLog);
    }

    if (keyPressed->scancode == Scancode::D || keyPressed->scancode == Scancode::Right)
    {
        return tryMoveOrAttack({1, 0}, map, enemies, messageLog);
    }

    return false;
}

void Player::addItem(const Item& item)
{
    m_inventory.push_back(item);
}

void Player::clearInventory()
{
    m_inventory.clear();
}

bool Player::useInventoryItem(std::size_t index, MessageLog& messageLog)
{
    if (index >= m_inventory.size())
    {
        return false;
    }

    const Item item = m_inventory[index];

    switch (item.getType())
    {
        case ItemType::Potion:
            heal(10);
            messageLog.add("You drink " + item.getName() + " and restore 10 HP.");
            break;

        case ItemType::Weapon:
            increaseDamage(2);
            messageLog.add("You equip " + item.getName() + ". Attack damage increased.");
            break;

        case ItemType::Armor:
            increaseMaxHp(5);
            messageLog.add("You equip " + item.getName() + ". Max HP increased.");
            break;
    }

    m_inventory.erase(m_inventory.begin() + static_cast<std::ptrdiff_t>(index));
    return true;
}

const std::vector<Item>& Player::getInventory() const
{
    return m_inventory;
}

bool Player::tryMoveOrAttack(
    const sf::Vector2i& direction,
    const Map& map,
    std::vector<Enemy>& enemies,
    MessageLog& messageLog
)
{
    const sf::Vector2i targetPosition = getGridPosition() + direction;

    for (Enemy& enemy : enemies)
    {
        if (!enemy.isAlive())
        {
            continue;
        }

        if (enemy.getGridPosition() == targetPosition)
        {
            enemy.takeDamage(getDamage());

            messageLog.add(
                "You hit " + enemy.getName() + " for " +
                std::to_string(getDamage()) + " damage."
            );

            if (!enemy.isAlive())
            {
                messageLog.add(enemy.getName() + " is defeated.");
            }

            return true;
        }
    }

    if (map.isWalkable(targetPosition.x, targetPosition.y))
    {
        setGridPosition(targetPosition);
        return true;
    }

    // Yön tuşuna basıldıysa, duvara çarpmak bile bir turn harcar.
    return true;
}
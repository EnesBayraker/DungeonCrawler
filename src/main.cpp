#include <SFML/Graphics.hpp>

#include <algorithm>
#include <cstddef>
#include <cstdlib>
#include <random>
#include <vector>

#include "Enemy.h"
#include "GameUI.h"
#include "Item.h"
#include "Map.h"
#include "MessageLog.h"
#include "Player.h"

int randomInt(std::mt19937& randomEngine, int min, int max)
{
    std::uniform_int_distribution<int> distribution(min, max);
    return distribution(randomEngine);
}

int getManhattanDistance(const sf::Vector2i& first, const sf::Vector2i& second)
{
    return std::abs(first.x - second.x) + std::abs(first.y - second.y);
}

bool isEnemyAtPosition(const std::vector<Enemy>& enemies, const sf::Vector2i& position)
{
    for (const Enemy& enemy : enemies)
    {
        if (enemy.isAlive() && enemy.getGridPosition() == position)
        {
            return true;
        }
    }

    return false;
}

bool isItemAtPosition(const std::vector<Item>& items, const sf::Vector2i& position)
{
    for (const Item& item : items)
    {
        if (item.getGridPosition() == position)
        {
            return true;
        }
    }

    return false;
}

ItemType getRandomItemType(std::mt19937& randomEngine)
{
    const int roll = randomInt(randomEngine, 0, 2);

    if (roll == 0)
    {
        return ItemType::Potion;
    }

    if (roll == 1)
    {
        return ItemType::Weapon;
    }

    return ItemType::Armor;
}

std::vector<Enemy> createEnemies(const Map& map, const sf::Vector2i& playerPosition)
{
    std::vector<Enemy> enemies;
    std::mt19937 randomEngine(std::random_device{}());

    constexpr int MaxEnemyCount = 6;

    for (const Room& room : map.getRooms())
    {
        if (static_cast<int>(enemies.size()) >= MaxEnemyCount)
        {
            break;
        }

        const sf::Vector2i spawnPosition = room.getCenter();

        if (spawnPosition == playerPosition)
        {
            continue;
        }

        if (randomInt(randomEngine, 0, 100) < 60)
        {
            enemies.emplace_back(spawnPosition);
        }
    }

    return enemies;
}

std::vector<Item> createItems(
    const Map& map,
    const sf::Vector2i& playerPosition,
    const std::vector<Enemy>& enemies
)
{
    std::vector<Item> items;
    std::mt19937 randomEngine(std::random_device{}());

    constexpr int MaxItemCount = 8;

    for (const Room& room : map.getRooms())
    {
        if (static_cast<int>(items.size()) >= MaxItemCount)
        {
            break;
        }

        // Her odada item olmak zorunda değil.
        if (randomInt(randomEngine, 0, 100) > 65)
        {
            continue;
        }

        const sf::Vector2i itemPosition{
            randomInt(randomEngine, room.x, room.x + room.width - 1),
            randomInt(randomEngine, room.y, room.y + room.height - 1)
        };

        if (itemPosition == playerPosition)
        {
            continue;
        }

        if (isEnemyAtPosition(enemies, itemPosition))
        {
            continue;
        }

        if (isItemAtPosition(items, itemPosition))
        {
            continue;
        }

        items.emplace_back(getRandomItemType(randomEngine), itemPosition);
    }

    return items;
}

void tryPickupItem(
    std::vector<Item>& items,
    const Player& player,
    MessageLog& messageLog
)
{
    const sf::Vector2i playerPosition = player.getGridPosition();

    auto itemIt = std::find_if(
        items.begin(),
        items.end(),
        [playerPosition](const Item& item)
        {
            return item.getGridPosition() == playerPosition;
        }
    );

    if (itemIt == items.end())
    {
        return;
    }

    messageLog.add("You picked up " + itemIt->getName() + ".");

    // Envanter sonradan eklenecek
    items.erase(itemIt);
}

void removeDeadEnemies(std::vector<Enemy>& enemies)
{
    enemies.erase(
        std::remove_if(
            enemies.begin(),
            enemies.end(),
            [](const Enemy& enemy)
            {
                return !enemy.isAlive();
            }
        ),
        enemies.end()
    );
}

std::vector<sf::Vector2i> collectEnemyPositions(
    const std::vector<Enemy>& enemies,
    std::size_t ignoredIndex
)
{
    std::vector<sf::Vector2i> positions;

    for (std::size_t i = 0; i < enemies.size(); ++i)
    {
        if (i == ignoredIndex)
        {
            continue;
        }

        positions.push_back(enemies[i].getGridPosition());
    }

    return positions;
}

std::vector<bool> calculateGroupAlertStates(
    const std::vector<Enemy>& enemies,
    const Map& map,
    const sf::Vector2i& playerPosition
)
{
    constexpr int EnemySightRadius = 6;
    constexpr int GroupAlertRadius = 5;

    std::vector<bool> forceChase(enemies.size(), false);

    for (std::size_t i = 0; i < enemies.size(); ++i)
    {
        const sf::Vector2i observerPosition = enemies[i].getGridPosition();

        if (!map.canSee(observerPosition, playerPosition, EnemySightRadius))
        {
            continue;
        }

        // Oyuncuyu gören düşman, yakınındaki diğer düşmanları da alarma geçirir.
        for (std::size_t j = 0; j < enemies.size(); ++j)
        {
            const sf::Vector2i allyPosition = enemies[j].getGridPosition();

            if (getManhattanDistance(observerPosition, allyPosition) <= GroupAlertRadius)
            {
                forceChase[j] = true;
            }
        }
    }

    return forceChase;
}

void updateEnemies(
    std::vector<Enemy>& enemies,
    const Map& map,
    Player& player,
    MessageLog& messageLog
)
{
    const sf::Vector2i playerPosition = player.getGridPosition();
    const std::vector<bool> forceChase = calculateGroupAlertStates(enemies, map, playerPosition);

    for (std::size_t i = 0; i < enemies.size(); ++i)
    {
        if (!player.isAlive())
        {
            return;
        }

        const std::vector<sf::Vector2i> occupiedPositions = collectEnemyPositions(enemies, i);

        enemies[i].updateAI(
            map,
            player,
            occupiedPositions,
            forceChase[i],
            messageLog
        );
    }
}

int main()
{
    sf::RenderWindow window(
        sf::VideoMode({800, 720}),
        "Roguelike Dungeon Crawler"
    );

    window.setFramerateLimit(60);
    window.setKeyRepeatEnabled(false);

    Map map;
    Player player;
    MessageLog messageLog;
    GameUI gameUI;

    player.setGridPosition(map.getPlayerStart());

    std::vector<Enemy> enemies = createEnemies(map, player.getGridPosition());
    std::vector<Item> items = createItems(map, player.getGridPosition(), enemies);

    messageLog.add("Welcome to the dungeon.");

    while (window.isOpen())
    {
        bool playerTookTurn = false;

        while (const auto event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }

            if (
                player.isAlive() &&
                player.handleInput(*event, map, enemies, messageLog)
            )
            {
                playerTookTurn = true;
            }
        }

        if (playerTookTurn && player.isAlive())
        {
            tryPickupItem(items, player, messageLog);

            removeDeadEnemies(enemies);
            updateEnemies(enemies, map, player, messageLog);
            removeDeadEnemies(enemies);
        }

        map.computeFov(player.getGridPosition(), 6);

        window.clear(sf::Color::Black);

        map.draw(window);

        for (const Item& item : items)
        {
            const sf::Vector2i itemPosition = item.getGridPosition();

            // Fog of War nedeniyle sadece görünen item'ları çiziyoruz.
            if (map.isVisible(itemPosition.x, itemPosition.y))
            {
                item.draw(window);
            }
        }

        for (const Enemy& enemy : enemies)
        {
            const sf::Vector2i enemyPosition = enemy.getGridPosition();

            if (map.isVisible(enemyPosition.x, enemyPosition.y))
            {
                enemy.draw(window);
            }
        }

        player.draw(window);
        gameUI.draw(window, player, messageLog);

        window.display();
    }

    return 0;
}
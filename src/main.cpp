#include <SFML/Graphics.hpp>

#include <algorithm>
#include <cstddef>
#include <cstdlib>
#include <random>
#include <string>
#include <vector>

#include "Enemy.h"
#include "GameUI.h"
#include "Item.h"
#include "Map.h"
#include "MessageLog.h"
#include "Player.h"
#include "SaveSystem.h"

enum class GameState
{
    MainMenu,
    Playing,
    GameOver
};

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

bool isInventoryToggleKey(const sf::Event& event)
{
    const auto* keyPressed = event.getIf<sf::Event::KeyPressed>();

    if (keyPressed == nullptr)
    {
        return false;
    }

    return keyPressed->scancode == sf::Keyboard::Scancode::I;
}

bool isInventoryCloseKey(const sf::Event& event)
{
    const auto* keyPressed = event.getIf<sf::Event::KeyPressed>();

    if (keyPressed == nullptr)
    {
        return false;
    }

    return keyPressed->scancode == sf::Keyboard::Scancode::Escape ||
           keyPressed->scancode == sf::Keyboard::Scancode::I;
}

bool isSaveKey(const sf::Event& event)
{
    const auto* keyPressed = event.getIf<sf::Event::KeyPressed>();

    if (keyPressed == nullptr)
    {
        return false;
    }

    return keyPressed->scancode == sf::Keyboard::Scancode::F5;
}

int getInventorySelectionIndex(const sf::Event& event)
{
    const auto* keyPressed = event.getIf<sf::Event::KeyPressed>();

    if (keyPressed == nullptr)
    {
        return -1;
    }

    using Scancode = sf::Keyboard::Scancode;

    switch (keyPressed->scancode)
    {
        case Scancode::Num1:
            return 0;
        case Scancode::Num2:
            return 1;
        case Scancode::Num3:
            return 2;
        case Scancode::Num4:
            return 3;
        case Scancode::Num5:
            return 4;
        case Scancode::Num6:
            return 5;
        case Scancode::Num7:
            return 6;
        case Scancode::Num8:
            return 7;
        case Scancode::Num9:
            return 8;
        default:
            return -1;
    }
}

bool isStartNewGameKey(const sf::Event& event)
{
    const auto* keyPressed = event.getIf<sf::Event::KeyPressed>();

    if (keyPressed == nullptr)
    {
        return false;
    }

    return keyPressed->scancode == sf::Keyboard::Scancode::Enter;
}

bool isLoadGameKey(const sf::Event& event)
{
    const auto* keyPressed = event.getIf<sf::Event::KeyPressed>();

    if (keyPressed == nullptr)
    {
        return false;
    }

    return keyPressed->scancode == sf::Keyboard::Scancode::L;
}

bool isExitKey(const sf::Event& event)
{
    const auto* keyPressed = event.getIf<sf::Event::KeyPressed>();

    if (keyPressed == nullptr)
    {
        return false;
    }

    return keyPressed->scancode == sf::Keyboard::Scancode::Escape;
}

bool isReturnToMenuKey(const sf::Event& event)
{
    const auto* keyPressed = event.getIf<sf::Event::KeyPressed>();

    if (keyPressed == nullptr)
    {
        return false;
    }

    return keyPressed->scancode == sf::Keyboard::Scancode::Enter;
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

        if (spawnPosition == map.getStairsPosition())
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

        if (itemPosition == map.getStairsPosition())
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

void startNewGame(
    Map& map,
    Player& player,
    std::vector<Enemy>& enemies,
    std::vector<Item>& items,
    MessageLog& messageLog
)
{
    map.generateNewFloor();

    player.setCombatStats(30, 30, 5);
    player.clearInventory();
    player.setGridPosition(map.getPlayerStart());

    enemies = createEnemies(map, player.getGridPosition());
    items = createItems(map, player.getGridPosition(), enemies);

    messageLog = MessageLog();
    messageLog.add("New game started.");
}

void startNextFloor(
    Map& map,
    Player& player,
    std::vector<Enemy>& enemies,
    std::vector<Item>& items,
    MessageLog& messageLog
)
{
    map.generateNewFloor();

    player.setGridPosition(map.getPlayerStart());

    enemies = createEnemies(map, player.getGridPosition());
    items = createItems(map, player.getGridPosition(), enemies);

    messageLog.add("You descend to a deeper floor.");
}

void tryPickupItem(
    std::vector<Item>& items,
    Player& player,
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

    player.addItem(*itemIt);

    messageLog.add("You picked up " + itemIt->getName() + ".");

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

    std::vector<Enemy> enemies;
    std::vector<Item> items;

    GameState gameState = GameState::MainMenu;
    bool inventoryOpen = false;

    std::string menuStatus = "Enter: new game | L: load save | Escape: exit";

    while (window.isOpen())
    {
        bool playerTookTurn = false;

        while (const auto event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }

            if (gameState == GameState::MainMenu)
            {
                if (isStartNewGameKey(*event))
                {
                    startNewGame(map, player, enemies, items, messageLog);
                    inventoryOpen = false;
                    gameState = GameState::Playing;
                    continue;
                }

                if (isLoadGameKey(*event))
                {
                    messageLog = MessageLog();

                    if (
                        SaveSystem::loadGame(
                            "savegame.txt",
                            map,
                            player,
                            enemies,
                            items,
                            messageLog
                        )
                    )
                    {
                        inventoryOpen = false;
                        gameState = GameState::Playing;
                    }
                    else
                    {
                        menuStatus = "Load failed. Make sure savegame.txt exists.";
                    }

                    continue;
                }

                if (isExitKey(*event))
                {
                    window.close();
                    continue;
                }

                continue;
            }

            if (gameState == GameState::GameOver)
            {
                if (isReturnToMenuKey(*event))
                {
                    inventoryOpen = false;
                    menuStatus = "Enter: new game | L: load save | Escape: exit";
                    gameState = GameState::MainMenu;
                    continue;
                }

                if (isExitKey(*event))
                {
                    window.close();
                    continue;
                }

                continue;
            }

            if (inventoryOpen)
            {
                if (isInventoryCloseKey(*event))
                {
                    inventoryOpen = false;
                    continue;
                }

                const int selectedIndex = getInventorySelectionIndex(*event);

                if (
                    selectedIndex >= 0 &&
                    player.isAlive() &&
                    player.useInventoryItem(static_cast<std::size_t>(selectedIndex), messageLog)
                )
                {
                    inventoryOpen = false;
                    playerTookTurn = true;
                }

                continue;
            }

            if (isSaveKey(*event))
            {
                SaveSystem::saveGame(
                    "savegame.txt",
                    map,
                    player,
                    enemies,
                    items,
                    messageLog
                );

                continue;
            }

            if (isInventoryToggleKey(*event))
            {
                inventoryOpen = true;
                continue;
            }

            if (
                player.isAlive() &&
                player.handleInput(*event, map, enemies, messageLog)
            )
            {
                playerTookTurn = true;
            }
        }

        if (
    gameState == GameState::Playing &&
    playerTookTurn &&
    player.isAlive() &&
    !inventoryOpen
)
        {
            tryPickupItem(items, player, messageLog);

            if (player.getGridPosition() == map.getStairsPosition())
            {
                startNextFloor(map, player, enemies, items, messageLog);
            }
            else
            {
                removeDeadEnemies(enemies);
                updateEnemies(enemies, map, player, messageLog);
                removeDeadEnemies(enemies);
            }

            if (!player.isAlive())
            {
                inventoryOpen = false;
                gameState = GameState::GameOver;
            }
        }

        if (gameState == GameState::Playing || gameState == GameState::GameOver)
        {
            map.computeFov(player.getGridPosition(), 6);
        }

        window.clear(sf::Color::Black);

        if (gameState == GameState::MainMenu)
        {
            gameUI.drawMainMenu(window, menuStatus);
            window.display();
            continue;
        }

        map.draw(window);

        for (const Item& item : items)
        {
            const sf::Vector2i itemPosition = item.getGridPosition();

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

        if (inventoryOpen)
        {
            gameUI.drawInventory(window, player);
        }

        if (gameState == GameState::GameOver)
        {
            gameUI.drawGameOver(window, player);
        }

        window.display();
    }

    return 0;
}
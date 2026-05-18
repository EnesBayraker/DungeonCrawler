#include "Game.h"

#include <algorithm>
#include <cstdlib>

#include "SaveSystem.h"

Game::Game()
    : m_window(sf::VideoMode({800, 720}), "Roguelike Dungeon Crawler"),
      m_gameState(GameState::MainMenu),
      m_inventoryOpen(false),
      m_menuStatus("Enter: new game | L: load save | Escape: exit")
{
    m_window.setFramerateLimit(60);
    m_window.setKeyRepeatEnabled(false);
}

void Game::run()
{
    while (m_window.isOpen())
    {
        processEvents();

        if (m_gameState == GameState::Playing || m_gameState == GameState::GameOver)
        {
            m_map.computeFov(m_player.getGridPosition(), 6);
        }

        render();
    }
}

void Game::processEvents()
{
    bool playerTookTurn = false;

    while (const auto event = m_window.pollEvent())
    {
        if (event->is<sf::Event::Closed>())
        {
            m_window.close();
        }

        if (m_gameState == GameState::MainMenu)
        {
            handleMainMenuEvent(*event);
            continue;
        }

        if (m_gameState == GameState::GameOver)
        {
            handleGameOverEvent(*event);
            continue;
        }

        if (m_inventoryOpen)
        {
            if (handleInventoryEvent(*event))
            {
                playerTookTurn = true;
            }

            continue;
        }

        if (handlePlayingEvent(*event))
        {
            playerTookTurn = true;
        }
    }

    updateAfterPlayerTurn(playerTookTurn);
}

void Game::updateAfterPlayerTurn(bool playerTookTurn)
{
    if (
        m_gameState != GameState::Playing ||
        !playerTookTurn ||
        !m_player.isAlive() ||
        m_inventoryOpen
    )
    {
        return;
    }

    tryPickupItem();

    if (m_player.getGridPosition() == m_map.getStairsPosition())
    {
        startNextFloor();
        return;
    }

    removeDeadEnemies();
    updateEnemies();
    removeDeadEnemies();

    if (!m_player.isAlive())
    {
        m_inventoryOpen = false;
        m_gameState = GameState::GameOver;
    }
}

void Game::render()
{
    m_window.clear(sf::Color::Black);

    if (m_gameState == GameState::MainMenu)
    {
        m_gameUI.drawMainMenu(m_window, m_menuStatus);
        m_window.display();
        return;
    }

    m_map.draw(m_window);

    for (const Item& item : m_items)
    {
        const sf::Vector2i itemPosition = item.getGridPosition();

        if (m_map.isVisible(itemPosition.x, itemPosition.y))
        {
            item.draw(m_window);
        }
    }

    for (const Enemy& enemy : m_enemies)
    {
        const sf::Vector2i enemyPosition = enemy.getGridPosition();

        if (m_map.isVisible(enemyPosition.x, enemyPosition.y))
        {
            enemy.draw(m_window);
        }
    }

    m_player.draw(m_window);
    m_gameUI.draw(m_window, m_player, m_messageLog);

    if (m_inventoryOpen)
    {
        m_gameUI.drawInventory(m_window, m_player);
    }

    if (m_gameState == GameState::GameOver)
    {
        m_gameUI.drawGameOver(m_window, m_player);
    }

    m_window.display();
}

void Game::handleMainMenuEvent(const sf::Event& event)
{
    if (isStartNewGameKey(event))
    {
        startNewGame();
        m_inventoryOpen = false;
        m_gameState = GameState::Playing;
        return;
    }

    if (isLoadGameKey(event))
    {
        m_messageLog = MessageLog();

        if (
            SaveSystem::loadGame(
                "savegame.txt",
                m_map,
                m_player,
                m_enemies,
                m_items,
                m_messageLog
            )
        )
        {
            m_inventoryOpen = false;
            m_gameState = GameState::Playing;
        }
        else
        {
            m_menuStatus = "Load failed. Make sure savegame.txt exists.";
        }

        return;
    }

    if (isExitKey(event))
    {
        m_window.close();
    }
}

void Game::handleGameOverEvent(const sf::Event& event)
{
    if (isReturnToMenuKey(event))
    {
        m_inventoryOpen = false;
        m_menuStatus = "Enter: new game | L: load save | Escape: exit";
        m_gameState = GameState::MainMenu;
        return;
    }

    if (isExitKey(event))
    {
        m_window.close();
    }
}

bool Game::handleInventoryEvent(const sf::Event& event)
{
    if (isInventoryCloseKey(event))
    {
        m_inventoryOpen = false;
        return false;
    }

    const int selectedIndex = getInventorySelectionIndex(event);

    if (
        selectedIndex >= 0 &&
        m_player.isAlive() &&
        m_player.useInventoryItem(static_cast<std::size_t>(selectedIndex), m_messageLog)
    )
    {
        m_inventoryOpen = false;
        return true;
    }

    return false;
}

bool Game::handlePlayingEvent(const sf::Event& event)
{
    if (isSaveKey(event))
    {
        SaveSystem::saveGame(
            "savegame.txt",
            m_map,
            m_player,
            m_enemies,
            m_items,
            m_messageLog
        );

        return false;
    }

    if (isInventoryToggleKey(event))
    {
        m_inventoryOpen = true;
        return false;
    }

    if (
        m_player.isAlive() &&
        m_player.handleInput(event, m_map, m_enemies, m_messageLog)
    )
    {
        return true;
    }

    return false;
}

void Game::startNewGame()
{
    m_map.generateNewFloor();

    m_player.setCombatStats(30, 30, 5);
    m_player.clearInventory();
    m_player.setGridPosition(m_map.getPlayerStart());

    m_enemies = createEnemies(m_player.getGridPosition());
    m_items = createItems(m_player.getGridPosition(), m_enemies);

    m_messageLog = MessageLog();
    m_messageLog.add("New game started.");
}

void Game::startNextFloor()
{
    m_map.generateNewFloor();

    m_player.setGridPosition(m_map.getPlayerStart());

    m_enemies = createEnemies(m_player.getGridPosition());
    m_items = createItems(m_player.getGridPosition(), m_enemies);

    m_messageLog.add("You descend to a deeper floor.");
}

void Game::tryPickupItem()
{
    const sf::Vector2i playerPosition = m_player.getGridPosition();

    auto itemIt = std::find_if(
        m_items.begin(),
        m_items.end(),
        [playerPosition](const Item& item)
        {
            return item.getGridPosition() == playerPosition;
        }
    );

    if (itemIt == m_items.end())
    {
        return;
    }

    m_player.addItem(*itemIt);
    m_messageLog.add("You picked up " + itemIt->getName() + ".");

    m_items.erase(itemIt);
}

void Game::removeDeadEnemies()
{
    m_enemies.erase(
        std::remove_if(
            m_enemies.begin(),
            m_enemies.end(),
            [](const Enemy& enemy)
            {
                return !enemy.isAlive();
            }
        ),
        m_enemies.end()
    );
}

void Game::updateEnemies()
{
    const sf::Vector2i playerPosition = m_player.getGridPosition();
    const std::vector<bool> forceChase = calculateGroupAlertStates(playerPosition);

    for (std::size_t i = 0; i < m_enemies.size(); ++i)
    {
        if (!m_player.isAlive())
        {
            return;
        }

        const std::vector<sf::Vector2i> occupiedPositions = collectEnemyPositions(i);

        m_enemies[i].updateAI(
            m_map,
            m_player,
            occupiedPositions,
            forceChase[i],
            m_messageLog
        );
    }
}

std::vector<Enemy> Game::createEnemies(const sf::Vector2i& playerPosition) const
{
    std::vector<Enemy> enemies;
    std::mt19937 randomEngine(std::random_device{}());

    constexpr int MaxEnemyCount = 6;

    for (const Room& room : m_map.getRooms())
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

        if (spawnPosition == m_map.getStairsPosition())
        {
            continue;
        }

        if (randomInt(randomEngine, 0, 100) < 60)
        {
            enemies.emplace_back(getRandomEnemyType(randomEngine), spawnPosition);
        }
    }

    return enemies;
}

std::vector<Item> Game::createItems(
    const sf::Vector2i& playerPosition,
    const std::vector<Enemy>& enemies
) const
{
    std::vector<Item> items;
    std::mt19937 randomEngine(std::random_device{}());

    constexpr int MaxItemCount = 8;

    for (const Room& room : m_map.getRooms())
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

        if (itemPosition == m_map.getStairsPosition())
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

std::vector<sf::Vector2i> Game::collectEnemyPositions(std::size_t ignoredIndex) const
{
    std::vector<sf::Vector2i> positions;

    for (std::size_t i = 0; i < m_enemies.size(); ++i)
    {
        if (i == ignoredIndex)
        {
            continue;
        }

        positions.push_back(m_enemies[i].getGridPosition());
    }

    return positions;
}

std::vector<bool> Game::calculateGroupAlertStates(const sf::Vector2i& playerPosition) const
{
    constexpr int EnemySightRadius = 6;
    constexpr int GroupAlertRadius = 5;

    std::vector<bool> forceChase(m_enemies.size(), false);

    for (std::size_t i = 0; i < m_enemies.size(); ++i)
    {
        const sf::Vector2i observerPosition = m_enemies[i].getGridPosition();

        if (!m_map.canSee(observerPosition, playerPosition, EnemySightRadius))
        {
            continue;
        }

        for (std::size_t j = 0; j < m_enemies.size(); ++j)
        {
            const sf::Vector2i allyPosition = m_enemies[j].getGridPosition();

            if (getManhattanDistance(observerPosition, allyPosition) <= GroupAlertRadius)
            {
                forceChase[j] = true;
            }
        }
    }

    return forceChase;
}

ItemType Game::getRandomItemType(std::mt19937& randomEngine) const
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

EnemyType Game::getRandomEnemyType(std::mt19937& randomEngine) const
{
    const int roll = randomInt(randomEngine, 0, 100);

    if (roll < 50)
    {
        return EnemyType::Goblin;
    }

    if (roll < 80)
    {
        return EnemyType::Skeleton;
    }

    return EnemyType::Orc;
}

bool Game::isEnemyAtPosition(const std::vector<Enemy>& enemies, const sf::Vector2i& position) const
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

bool Game::isItemAtPosition(const std::vector<Item>& items, const sf::Vector2i& position) const
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

int Game::randomInt(std::mt19937& randomEngine, int min, int max) const
{
    std::uniform_int_distribution<int> distribution(min, max);
    return distribution(randomEngine);
}

int Game::getManhattanDistance(const sf::Vector2i& first, const sf::Vector2i& second) const
{
    return std::abs(first.x - second.x) + std::abs(first.y - second.y);
}

bool Game::isInventoryToggleKey(const sf::Event& event) const
{
    const auto* keyPressed = event.getIf<sf::Event::KeyPressed>();

    if (keyPressed == nullptr)
    {
        return false;
    }

    return keyPressed->scancode == sf::Keyboard::Scancode::I;
}

bool Game::isInventoryCloseKey(const sf::Event& event) const
{
    const auto* keyPressed = event.getIf<sf::Event::KeyPressed>();

    if (keyPressed == nullptr)
    {
        return false;
    }

    return keyPressed->scancode == sf::Keyboard::Scancode::Escape ||
           keyPressed->scancode == sf::Keyboard::Scancode::I;
}

bool Game::isSaveKey(const sf::Event& event) const
{
    const auto* keyPressed = event.getIf<sf::Event::KeyPressed>();

    if (keyPressed == nullptr)
    {
        return false;
    }

    return keyPressed->scancode == sf::Keyboard::Scancode::F5;
}

bool Game::isStartNewGameKey(const sf::Event& event) const
{
    const auto* keyPressed = event.getIf<sf::Event::KeyPressed>();

    if (keyPressed == nullptr)
    {
        return false;
    }

    return keyPressed->scancode == sf::Keyboard::Scancode::Enter;
}

bool Game::isLoadGameKey(const sf::Event& event) const
{
    const auto* keyPressed = event.getIf<sf::Event::KeyPressed>();

    if (keyPressed == nullptr)
    {
        return false;
    }

    return keyPressed->scancode == sf::Keyboard::Scancode::L;
}

bool Game::isExitKey(const sf::Event& event) const
{
    const auto* keyPressed = event.getIf<sf::Event::KeyPressed>();

    if (keyPressed == nullptr)
    {
        return false;
    }

    return keyPressed->scancode == sf::Keyboard::Scancode::Escape;
}

bool Game::isReturnToMenuKey(const sf::Event& event) const
{
    const auto* keyPressed = event.getIf<sf::Event::KeyPressed>();

    if (keyPressed == nullptr)
    {
        return false;
    }

    return keyPressed->scancode == sf::Keyboard::Scancode::Enter;
}

int Game::getInventorySelectionIndex(const sf::Event& event) const
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
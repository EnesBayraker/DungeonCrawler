#pragma once

#include <SFML/Graphics.hpp>

#include <cstddef>
#include <random>
#include <string>
#include <vector>
#include "AudioManager.h"
#include "Enemy.h"
#include "GameUI.h"
#include "Item.h"
#include "Map.h"
#include "MessageLog.h"
#include "Player.h"

enum class GameState
{
    MainMenu,
    Help,
    Playing,
    Paused,
    GameOver,
    Victory
};

class Game
{
public:
    Game();

    void run();

private:
    static constexpr unsigned int BaseWidth = 800;
    static constexpr unsigned int BaseHeight = 720;
    static constexpr unsigned int InitialWindowWidth = 1000;
    static constexpr unsigned int InitialWindowHeight = 900;

    sf::RenderWindow m_window;
    sf::View m_gameView;

    Map m_map;
    Player m_player;
    MessageLog m_messageLog;
    GameUI m_gameUI;

    std::vector<Enemy> m_enemies;
    std::vector<Item> m_items;

    static constexpr int MaxFloor = 5;
    int m_floorNumber;
    AudioManager m_audioManager;
    GameState m_gameState;
    bool m_inventoryOpen;
    std::string m_menuStatus;

    void processEvents();
    void updateAfterPlayerTurn(bool playerTookTurn);
    void render();
    void updateView();

    void handleMainMenuEvent(const sf::Event& event);
    void handleGameOverEvent(const sf::Event& event);
    bool handleInventoryEvent(const sf::Event& event);
    bool handlePlayingEvent(const sf::Event& event);
    void handleHelpEvent(const sf::Event& event);
    void handlePausedEvent(const sf::Event& event);
    void handleVictoryEvent(const sf::Event& event);
    void startNewGame();
    void startNextFloor();

    void tryPickupItem();
    void removeDeadEnemiesAndDropItems();
    void updateEnemies();

    std::vector<Enemy> createEnemies(const sf::Vector2i& playerPosition) const;
    std::vector<Item> createItems(const sf::Vector2i& playerPosition, const std::vector<Enemy>& enemies) const;

    std::vector<sf::Vector2i> collectEnemyPositions(std::size_t ignoredIndex) const;
    std::vector<bool> calculateGroupAlertStates(const sf::Vector2i& playerPosition) const;

    ItemType getRandomItemType(std::mt19937& randomEngine) const;
    EnemyType getRandomEnemyType(std::mt19937& randomEngine) const;

    bool isEnemyAtPosition(const std::vector<Enemy>& enemies, const sf::Vector2i& position) const;
    bool isItemAtPosition(const std::vector<Item>& items, const sf::Vector2i& position) const;

    int randomInt(std::mt19937& randomEngine, int min, int max) const;
    int getManhattanDistance(const sf::Vector2i& first, const sf::Vector2i& second) const;

    bool isInventoryToggleKey(const sf::Event& event) const;
    bool isInventoryCloseKey(const sf::Event& event) const;
    bool isSaveKey(const sf::Event& event) const;
    bool isStartNewGameKey(const sf::Event& event) const;
    bool isLoadGameKey(const sf::Event& event) const;
    bool isExitKey(const sf::Event& event) const;
    bool isReturnToMenuKey(const sf::Event& event) const;
    bool isHelpKey(const sf::Event& event) const;

    int getInventorySelectionIndex(const sf::Event& event) const;
};
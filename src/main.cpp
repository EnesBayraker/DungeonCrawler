#include <SFML/Graphics.hpp>

#include <cstddef>
#include <cstdlib>
#include <random>
#include <vector>

#include "Enemy.h"
#include "Map.h"
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

        // Her uygun odaya düşman koymuyoruz; biraz rastgelelik bırakıyoruz.
        if (randomInt(randomEngine, 0, 100) < 60)
        {
            enemies.emplace_back(spawnPosition);
        }
    }

    return enemies;
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

void updateEnemies(std::vector<Enemy>& enemies, const Map& map, const Player& player)
{
    const sf::Vector2i playerPosition = player.getGridPosition();
    const std::vector<bool> forceChase = calculateGroupAlertStates(enemies, map, playerPosition);

    for (std::size_t i = 0; i < enemies.size(); ++i)
    {
        const std::vector<sf::Vector2i> occupiedPositions = collectEnemyPositions(enemies, i);

        enemies[i].updateAI(
            map,
            playerPosition,
            occupiedPositions,
            forceChase[i]
        );
    }
}

int main()
{
    sf::RenderWindow window(
        sf::VideoMode({800, 600}),
        "Roguelike Dungeon Crawler"
    );

    window.setFramerateLimit(60);
    window.setKeyRepeatEnabled(false);

    Map map;
    Player player;

    player.setGridPosition(map.getPlayerStart());

    std::vector<Enemy> enemies = createEnemies(map, player.getGridPosition());

    while (window.isOpen())
    {
        bool playerTookTurn = false;

        while (const auto event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }

            if (player.handleInput(*event, map))
            {
                playerTookTurn = true;
            }
        }

        if (playerTookTurn)
        {
            updateEnemies(enemies, map, player);
        }

        map.computeFov(player.getGridPosition(), 6);

        window.clear(sf::Color::Black);

        map.draw(window);

        for (const Enemy& enemy : enemies)
        {
            const sf::Vector2i enemyPosition = enemy.getGridPosition();

            // Fog of War nedeniyle sadece görünen düşmanları çiziyoruz.
            if (map.isVisible(enemyPosition.x, enemyPosition.y))
            {
                enemy.draw(window);
            }
        }

        player.draw(window);

        window.display();
    }

    return 0;
}
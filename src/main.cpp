#include <SFML/Graphics.hpp>

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

int main()
{
    sf::RenderWindow window(
        sf::VideoMode({800, 600}),
        "Roguelike Dungeon Crawler"
    );

    window.setFramerateLimit(60);

    Map map;
    Player player;

    player.setGridPosition(map.getPlayerStart());

    std::vector<Enemy> enemies = createEnemies(map, player.getGridPosition());

    while (window.isOpen())
    {
        while (const auto event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }

            player.handleInput(*event, map);
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
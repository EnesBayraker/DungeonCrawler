#include <SFML/Graphics.hpp>

#include "Map.h"
#include "Player.h"

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
        player.draw(window);

        window.display();
    }

    return 0;
}
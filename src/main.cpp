#include <SFML/Graphics.hpp>

#include "Player.h"

int main()
{
    sf::RenderWindow window(
        sf::VideoMode({800, 600}),
        "Roguelike Dungeon Crawler"
    );

    window.setFramerateLimit(60);

    Player player;
    sf::Clock clock;

    while (window.isOpen())
    {
        const float deltaTime = clock.restart().asSeconds();

        while (const auto event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }
        }

        player.handleInput(deltaTime);

        window.clear(sf::Color::Black);
        player.draw(window);
        window.display();
    }

    return 0;
}
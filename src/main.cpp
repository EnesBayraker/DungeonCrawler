#include <SFML/Graphics.hpp>

int main()
{
    sf::RenderWindow window(
        sf::VideoMode({800, 600}),
        "Roguelike Dungeon Crawler"
    );

    window.setFramerateLimit(60);

    sf::RectangleShape playerShape({32.f, 32.f});
    playerShape.setFillColor(sf::Color::White);
    playerShape.setPosition({384.f, 284.f});

    while (window.isOpen())
    {
        while (const auto event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }
        }

        window.clear(sf::Color::Black);
        window.draw(playerShape);
        window.display();
    }

    return 0;
}
#pragma once

#include <SFML/Graphics.hpp>

class MessageLog;
class Player;

class GameUI
{
public:
    GameUI();

    void draw(sf::RenderWindow& window, const Player& player, const MessageLog& messageLog) const;
    void drawInventory(sf::RenderWindow& window, const Player& player) const;

private:
    sf::Font m_font;
    bool m_fontLoaded;

    bool loadFont();
};
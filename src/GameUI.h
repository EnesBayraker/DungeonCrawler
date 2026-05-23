#pragma once

#include <SFML/Graphics.hpp>

#include <string>

class MessageLog;
class Player;

class GameUI
{
public:
    GameUI();
    void drawHelpScreen(sf::RenderWindow& window) const;
    void draw(sf::RenderWindow& window, const Player& player, const MessageLog& messageLog, int floorNumber) const;
    void drawInventory(sf::RenderWindow& window, const Player& player) const;
    void drawMainMenu(sf::RenderWindow& window, const std::string& statusMessage) const;
    void drawGameOver(sf::RenderWindow& window, const Player& player) const;
    void drawVictory(sf::RenderWindow& window, const Player& player) const;

private:
    sf::Font m_font;
    bool m_fontLoaded;

    bool loadFont();
};
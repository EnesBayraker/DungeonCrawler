#include "GameUI.h"

#include <array>
#include <string>
#include <vector>

#include "Item.h"
#include "Map.h"
#include "MessageLog.h"
#include "Player.h"

GameUI::GameUI()
    : m_fontLoaded(loadFont()),
      m_menuBackgroundLoaded(false)
{
    m_menuBackgroundLoaded = m_menuBackgroundTexture.loadFromFile("assets/ui/menu_background.png");

    if (m_menuBackgroundLoaded)
    {
        m_menuBackgroundTexture.setSmooth(true);
    }
}

bool GameUI::loadFont()
{
    // Windows için sistem fontlarını deniyoruz.
    const std::array<std::string, 3> fontPaths{
        "C:/Windows/Fonts/arial.ttf",
        "C:/Windows/Fonts/segoeui.ttf",
        "assets/fonts/arial.ttf"
    };

    for (const std::string& path : fontPaths)
    {
        if (m_font.openFromFile(path))
        {
            return true;
        }
    }

    return false;
}

void GameUI::draw(sf::RenderWindow& window, const Player& player, const MessageLog& messageLog, int floorNumber) const
{
    constexpr float PanelHeight = 144.f;

    const float panelY = static_cast<float>(Map::Height * Map::TileSize);
    const float panelWidth = static_cast<float>(Map::Width * Map::TileSize);

    sf::RectangleShape panel({panelWidth, PanelHeight});
    panel.setPosition({0.f, panelY});
    panel.setFillColor(sf::Color(15, 15, 15));
    panel.setOutlineThickness(2.f);
    panel.setOutlineColor(sf::Color(70, 70, 70));

    window.draw(panel);

    if (!m_fontLoaded)
    {
        return;
    }

    sf::Text hpText(
    m_font,
    "Depth: " + std::to_string(floorNumber) +
    " | HP: " + std::to_string(player.getHp()) +
    "/" + std::to_string(player.getMaxHp()) +
    " | Attack: " + std::to_string(player.getDamage()) +
    " | Inventory: " + std::to_string(player.getInventory().size()) +
    " item(s) | I: Inventory | F5: Save",
    20
);

    hpText.setFillColor(sf::Color::White);
    hpText.setPosition({16.f, panelY + 10.f});

    window.draw(hpText);

    const std::vector<std::string>& messages = messageLog.getMessages();

    float messageY = panelY + 42.f;

    for (const std::string& message : messages)
    {
        sf::Text messageText(m_font, message, 16);
        messageText.setFillColor(sf::Color(210, 210, 210));
        messageText.setPosition({16.f, messageY});

        window.draw(messageText);

        messageY += 18.f;
    }
}

void GameUI::drawInventory(sf::RenderWindow& window, const Player& player) const
{
    if (!m_fontLoaded)
    {
        return;
    }

    sf::RectangleShape overlay({520.f, 420.f});
    overlay.setPosition({140.f, 90.f});
    overlay.setFillColor(sf::Color(10, 10, 10, 235));
    overlay.setOutlineThickness(3.f);
    overlay.setOutlineColor(sf::Color(180, 180, 180));

    window.draw(overlay);

    sf::Text titleText(m_font, "Inventory", 28);
    titleText.setFillColor(sf::Color::White);
    titleText.setPosition({165.f, 115.f});

    window.draw(titleText);

    sf::Text helpText(m_font, "Press 1-9 to use an item | Press I or Escape to close", 16);
    helpText.setFillColor(sf::Color(180, 180, 180));
    helpText.setPosition({165.f, 150.f});

    window.draw(helpText);

    const std::vector<Item>& inventory = player.getInventory();

    if (inventory.empty())
    {
        sf::Text emptyText(m_font, "Your inventory is empty.", 20);
        emptyText.setFillColor(sf::Color(220, 220, 220));
        emptyText.setPosition({165.f, 200.f});

        window.draw(emptyText);
        return;
    }

    float itemY = 200.f;

    for (std::size_t i = 0; i < inventory.size(); ++i)
    {
        const std::string line =
            std::to_string(i + 1) + ") " + inventory[i].getName();

        sf::Text itemText(m_font, line, 20);
        itemText.setFillColor(sf::Color(220, 220, 220));
        itemText.setPosition({165.f, itemY});

        window.draw(itemText);

        itemY += 28.f;
    }
}

void GameUI::drawMainMenu(sf::RenderWindow& window, const std::string& statusMessage) const
{
    if (!m_fontLoaded)
    {
        return;
    }

    if (m_menuBackgroundLoaded)
    {
        sf::Sprite backgroundSprite(m_menuBackgroundTexture);

        const sf::Vector2u textureSize = m_menuBackgroundTexture.getSize();

        if (textureSize.x > 0 && textureSize.y > 0)
        {
            backgroundSprite.setScale({
                800.f / static_cast<float>(textureSize.x),
                720.f / static_cast<float>(textureSize.y)
            });
        }

        backgroundSprite.setPosition({0.f, 0.f});
        window.draw(backgroundSprite);

        // Yazıların daha okunur olması için koyu overlay.
        sf::RectangleShape darkOverlay({800.f, 720.f});
        darkOverlay.setPosition({0.f, 0.f});
        darkOverlay.setFillColor(sf::Color(0, 0, 0, 120));
        window.draw(darkOverlay);
    }
    else
    {
        sf::RectangleShape background({800.f, 720.f});
        background.setPosition({0.f, 0.f});
        background.setFillColor(sf::Color(20, 20, 24));
        window.draw(background);
    }

    sf::Text titleShadow(m_font, "Roguelike Dungeon Crawler", 42);
    titleShadow.setFillColor(sf::Color(0, 0, 0, 200));
    titleShadow.setPosition({166.f, 116.f});
    window.draw(titleShadow);

    sf::Text titleText(m_font, "Roguelike Dungeon Crawler", 42);
    titleText.setFillColor(sf::Color(235, 235, 235));
    titleText.setPosition({162.f, 112.f});
    window.draw(titleText);

    sf::Text subtitleText(m_font, "Descend. Survive. Escape.", 22);
    subtitleText.setFillColor(sf::Color(210, 210, 210));
    subtitleText.setPosition({255.f, 165.f});
    window.draw(subtitleText);

    sf::RectangleShape menuPanel({360.f, 190.f});
    menuPanel.setPosition({220.f, 255.f});
    menuPanel.setFillColor(sf::Color(0, 0, 0, 120));
    menuPanel.setOutlineThickness(2.f);
    menuPanel.setOutlineColor(sf::Color(120, 120, 120, 160));
    window.draw(menuPanel);

    sf::Text startText(m_font, "Enter - Start New Game", 24);
    startText.setFillColor(sf::Color(235, 235, 235));
    startText.setPosition({245.f, 280.f});
    window.draw(startText);

    sf::Text loadText(m_font, "L - Load Game", 24);
    loadText.setFillColor(sf::Color(220, 220, 220));
    loadText.setPosition({245.f, 320.f});
    window.draw(loadText);

    sf::Text helpText(m_font, "H - How to Play", 24);
    helpText.setFillColor(sf::Color(220, 220, 220));
    helpText.setPosition({245.f, 360.f});
    window.draw(helpText);

    sf::Text exitText(m_font, "Escape - Exit", 24);
    exitText.setFillColor(sf::Color(220, 220, 220));
    exitText.setPosition({245.f, 400.f});
    window.draw(exitText);

    sf::Text statusText(m_font, statusMessage, 18);
    statusText.setFillColor(sf::Color(230, 220, 170));
    statusText.setPosition({120.f, 640.f});
    window.draw(statusText);
}

void GameUI::drawGameOver(sf::RenderWindow& window, const Player& player) const
{
    if (!m_fontLoaded)
    {
        return;
    }

    sf::RectangleShape overlay({800.f, 720.f});
    overlay.setPosition({0.f, 0.f});
    overlay.setFillColor(sf::Color(0, 0, 0, 190));

    window.draw(overlay);

    sf::Text titleText(m_font, "GAME OVER", 54);
    titleText.setFillColor(sf::Color(220, 60, 60));
    titleText.setPosition({250.f, 210.f});

    window.draw(titleText);

    sf::Text statsText(
        m_font,
        "Final HP: " + std::to_string(player.getHp()) +
        "/" + std::to_string(player.getMaxHp()) +
        " | Attack: " + std::to_string(player.getDamage()),
        22
    );

    statsText.setFillColor(sf::Color(220, 220, 220));
    statsText.setPosition({245.f, 295.f});

    window.draw(statsText);

    sf::Text menuText(m_font, "Enter - Return to Main Menu", 24);
    menuText.setFillColor(sf::Color::White);
    menuText.setPosition({245.f, 355.f});

    window.draw(menuText);

    sf::Text exitText(m_font, "Escape - Exit", 22);
    exitText.setFillColor(sf::Color(190, 190, 190));
    exitText.setPosition({245.f, 395.f});

    window.draw(exitText);
}

void GameUI::drawVictory(sf::RenderWindow& window, const Player& player) const
{
    if (!m_fontLoaded)
    {
        return;
    }

    sf::RectangleShape overlay({800.f, 720.f});
    overlay.setPosition({0.f, 0.f});
    overlay.setFillColor(sf::Color(0, 0, 0, 190));

    window.draw(overlay);

    sf::Text titleText(m_font, "VICTORY!", 56);
    titleText.setFillColor(sf::Color(80, 220, 120));
    titleText.setPosition({270.f, 210.f});

    window.draw(titleText);

    sf::Text statsText(
        m_font,
        "You escaped the dungeon with " +
        std::to_string(player.getHp()) + "/" +
        std::to_string(player.getMaxHp()) + " HP.",
        22
    );

    statsText.setFillColor(sf::Color(230, 230, 230));
    statsText.setPosition({210.f, 300.f});

    window.draw(statsText);

    sf::Text menuText(m_font, "Enter - Return to Main Menu", 24);
    menuText.setFillColor(sf::Color::White);
    menuText.setPosition({245.f, 365.f});

    window.draw(menuText);

    sf::Text exitText(m_font, "Escape - Exit", 22);
    exitText.setFillColor(sf::Color(190, 190, 190));
    exitText.setPosition({245.f, 405.f});

    window.draw(exitText);
}

void GameUI::drawHelpScreen(sf::RenderWindow& window) const
{
    if (!m_fontLoaded)
    {
        return;
    }

    sf::Text titleText(m_font, "How to Play", 42);
    titleText.setFillColor(sf::Color::White);
    titleText.setPosition({285.f, 80.f});

    window.draw(titleText);

    const std::vector<std::string> lines{
        "Movement:",
        "  W A S D / Arrow Keys - Move or attack",
        "",
        "Combat:",
        "  Move into an enemy to attack it",
        "  Enemies take a turn after your action",
        "  Walking into a wall also consumes a turn",
        "",
        "Inventory:",
        "  I - Open / close inventory",
        "  1-9 - Use selected inventory item",
        "",
        "Items:",
        "  Potion - Restore HP",
        "  Sword - Increase attack damage",
        "  Armor - Increase max HP",
        "",
        "Other:",
        "  F5 - Save game",
        "  L - Load game from main menu",
        "  Reach Depth 5 and use the stairs to win",
        "",
        "Press Enter or Escape to return"
    };

    float y = 150.f;

    for (const std::string& line : lines)
    {
        sf::Text text(m_font, line, 19);

        if (line == "Movement:" || line == "Combat:" || line == "Inventory:" ||
            line == "Items:" || line == "Other:")
        {
            text.setFillColor(sf::Color(120, 200, 255));
        }
        else
        {
            text.setFillColor(sf::Color(220, 220, 220));
        }

        text.setPosition({150.f, y});
        window.draw(text);

        y += 26.f;
    }
}
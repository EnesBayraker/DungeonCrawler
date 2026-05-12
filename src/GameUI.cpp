#include "GameUI.h"

#include <array>
#include <string>
#include <vector>

#include "Item.h"
#include "Map.h"
#include "MessageLog.h"
#include "Player.h"

GameUI::GameUI()
    : m_fontLoaded(loadFont())
{
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

void GameUI::draw(sf::RenderWindow& window, const Player& player, const MessageLog& messageLog) const
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
        "HP: " + std::to_string(player.getHp()) +
  "/" + std::to_string(player.getMaxHp()) +
  " | Attack: " + std::to_string(player.getDamage()) +
  " | Inventory: " + std::to_string(player.getInventory().size()) +
  " item(s) | Press I",
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
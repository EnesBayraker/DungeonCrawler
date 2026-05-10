#include "GameUI.h"

#include <array>
#include <string>

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
    // Daha sonra assets klasörü kullanırsak burayı değiştirebiliriz.
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
        " | Damage: " + std::to_string(player.getDamage()),
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
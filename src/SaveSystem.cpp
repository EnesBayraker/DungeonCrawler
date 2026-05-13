#include "SaveSystem.h"

#include <fstream>
#include <string>

#include "Enemy.h"
#include "Item.h"
#include "Map.h"
#include "MessageLog.h"
#include "Player.h"

namespace
{
    int itemTypeToInt(ItemType type)
    {
        switch (type)
        {
            case ItemType::Potion:
                return 0;
            case ItemType::Weapon:
                return 1;
            case ItemType::Armor:
                return 2;
        }

        return 0;
    }

    char tileToChar(TileType tile)
    {
        switch (tile)
        {
            case TileType::Floor:
                return '.';
            case TileType::Wall:
                return '#';
        }

        return '#';
    }
}

bool SaveSystem::saveGame(
    const std::string& filePath,
    const Map& map,
    const Player& player,
    const std::vector<Enemy>& enemies,
    const std::vector<Item>& items,
    MessageLog& messageLog
)
{
    std::ofstream file(filePath);

    if (!file.is_open())
    {
        messageLog.add("Save failed: could not open save file.");
        return false;
    }

    const sf::Vector2i playerPosition = player.getGridPosition();
    const sf::Vector2i stairsPosition = map.getStairsPosition();

    file << "SAVE_VERSION 1\n";

    file << "PLAYER "
         << playerPosition.x << ' '
         << playerPosition.y << ' '
         << player.getHp() << ' '
         << player.getMaxHp() << ' '
         << player.getDamage() << '\n';

    file << "STAIRS "
         << stairsPosition.x << ' '
         << stairsPosition.y << '\n';

    file << "MAP "
         << Map::Width << ' '
         << Map::Height << '\n';

    file << "TILES\n";

    for (int y = 0; y < Map::Height; ++y)
    {
        for (int x = 0; x < Map::Width; ++x)
        {
            file << tileToChar(map.getTile(x, y));
        }

        file << '\n';
    }

    file << "EXPLORED\n";

    for (int y = 0; y < Map::Height; ++y)
    {
        for (int x = 0; x < Map::Width; ++x)
        {
            file << (map.isExplored(x, y) ? '1' : '0');
        }

        file << '\n';
    }

    file << "INVENTORY " << player.getInventory().size() << '\n';

    for (const Item& item : player.getInventory())
    {
        file << itemTypeToInt(item.getType()) << '\n';
    }

    file << "ITEMS " << items.size() << '\n';

    for (const Item& item : items)
    {
        const sf::Vector2i itemPosition = item.getGridPosition();

        file << itemTypeToInt(item.getType()) << ' '
             << itemPosition.x << ' '
             << itemPosition.y << '\n';
    }

    file << "ENEMIES " << enemies.size() << '\n';

    for (const Enemy& enemy : enemies)
    {
        const sf::Vector2i enemyPosition = enemy.getGridPosition();

        file << enemyPosition.x << ' '
             << enemyPosition.y << ' '
             << enemy.getHp() << '\n';
    }

    messageLog.add("Game saved to " + filePath + ".");
    return true;
}
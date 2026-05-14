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

    ItemType intToItemType(int value)
    {
        switch (value)
        {
            case 0:
                return ItemType::Potion;
            case 1:
                return ItemType::Weapon;
            case 2:
                return ItemType::Armor;
            default:
                return ItemType::Potion;
        }
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

    TileType charToTile(char value)
    {
        if (value == '.')
        {
            return TileType::Floor;
        }

        return TileType::Wall;
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

bool SaveSystem::loadGame(
    const std::string& filePath,
    Map& map,
    Player& player,
    std::vector<Enemy>& enemies,
    std::vector<Item>& items,
    MessageLog& messageLog
)
{
    std::ifstream file(filePath);

    if (!file.is_open())
    {
        messageLog.add("Load failed: save file not found.");
        return false;
    }

    std::string label;
    int version = 0;

    file >> label >> version;

    if (label != "SAVE_VERSION" || version != 1)
    {
        messageLog.add("Load failed: invalid save version.");
        return false;
    }

    int playerX = 0;
    int playerY = 0;
    int playerHp = 0;
    int playerMaxHp = 0;
    int playerDamage = 0;

    file >> label >> playerX >> playerY >> playerHp >> playerMaxHp >> playerDamage;

    if (label != "PLAYER")
    {
        messageLog.add("Load failed: player data is missing.");
        return false;
    }

    int stairsX = 0;
    int stairsY = 0;

    file >> label >> stairsX >> stairsY;

    if (label != "STAIRS")
    {
        messageLog.add("Load failed: stairs data is missing.");
        return false;
    }

    int mapWidth = 0;
    int mapHeight = 0;

    file >> label >> mapWidth >> mapHeight;

    if (label != "MAP" || mapWidth != Map::Width || mapHeight != Map::Height)
    {
        messageLog.add("Load failed: map size mismatch.");
        return false;
    }

    file >> label;

    if (label != "TILES")
    {
        messageLog.add("Load failed: tile data is missing.");
        return false;
    }

    for (int y = 0; y < Map::Height; ++y)
    {
        std::string row;
        file >> row;

        if (static_cast<int>(row.size()) != Map::Width)
        {
            messageLog.add("Load failed: invalid tile row.");
            return false;
        }

        for (int x = 0; x < Map::Width; ++x)
        {
            map.setTile(x, y, charToTile(row[x]));
        }
    }

    file >> label;

    if (label != "EXPLORED")
    {
        messageLog.add("Load failed: explored data is missing.");
        return false;
    }

    for (int y = 0; y < Map::Height; ++y)
    {
        std::string row;
        file >> row;

        if (static_cast<int>(row.size()) != Map::Width)
        {
            messageLog.add("Load failed: invalid explored row.");
            return false;
        }

        for (int x = 0; x < Map::Width; ++x)
        {
            map.setExplored(x, y, row[x] == '1');
        }
    }

    map.setStairsPosition({stairsX, stairsY});

    player.setGridPosition({playerX, playerY});
    player.setCombatStats(playerHp, playerMaxHp, playerDamage);
    player.clearInventory();

    std::size_t inventoryCount = 0;

    file >> label >> inventoryCount;

    if (label != "INVENTORY")
    {
        messageLog.add("Load failed: inventory data is missing.");
        return false;
    }

    for (std::size_t i = 0; i < inventoryCount; ++i)
    {
        int itemTypeValue = 0;
        file >> itemTypeValue;

        player.addItem(Item(intToItemType(itemTypeValue), {0, 0}));
    }

    std::size_t itemCount = 0;

    file >> label >> itemCount;

    if (label != "ITEMS")
    {
        messageLog.add("Load failed: item data is missing.");
        return false;
    }

    items.clear();

    for (std::size_t i = 0; i < itemCount; ++i)
    {
        int itemTypeValue = 0;
        int itemX = 0;
        int itemY = 0;

        file >> itemTypeValue >> itemX >> itemY;

        items.emplace_back(intToItemType(itemTypeValue), sf::Vector2i{itemX, itemY});
    }

    std::size_t enemyCount = 0;

    file >> label >> enemyCount;

    if (label != "ENEMIES")
    {
        messageLog.add("Load failed: enemy data is missing.");
        return false;
    }

    enemies.clear();

    for (std::size_t i = 0; i < enemyCount; ++i)
    {
        int enemyX = 0;
        int enemyY = 0;
        int enemyHp = 0;

        file >> enemyX >> enemyY >> enemyHp;

        enemies.emplace_back(sf::Vector2i{enemyX, enemyY});
        enemies.back().setCombatStats(enemyHp, 10, 3);
    }

    messageLog.add("Game loaded from " + filePath + ".");
    return true;
}
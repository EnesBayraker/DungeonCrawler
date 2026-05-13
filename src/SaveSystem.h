#pragma once

#include <string>
#include <vector>

class Enemy;
class Item;
class Map;
class MessageLog;
class Player;

class SaveSystem
{
public:
    static bool saveGame(
        const std::string& filePath,
        const Map& map,
        const Player& player,
        const std::vector<Enemy>& enemies,
        const std::vector<Item>& items,
        MessageLog& messageLog
    );
};
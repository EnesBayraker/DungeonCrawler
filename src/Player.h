#pragma once

#include <SFML/Graphics.hpp>

#include <vector>

#include "Entity.h"

class Enemy;
class Map;

class Player : public Entity
{
public:
    Player();

    bool handleInput(const sf::Event& event, const Map& map, std::vector<Enemy>& enemies);

private:
    bool tryMoveOrAttack(
        const sf::Vector2i& direction,
        const Map& map,
        std::vector<Enemy>& enemies
    );
};
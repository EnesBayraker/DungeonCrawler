#pragma once

#include <SFML/Graphics.hpp>

#include "Entity.h"

class Map;

class Player : public Entity
{
public:
    Player();

    bool handleInput(const sf::Event& event, const Map& map);

private:
    bool tryMove(const sf::Vector2i& direction, const Map& map);
};
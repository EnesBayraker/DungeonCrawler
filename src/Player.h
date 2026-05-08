#pragma once

#include <SFML/Graphics.hpp>

#include "Entity.h"

class Map;

class Player : public Entity
{
public:
    Player();

    void handleInput(const sf::Event& event, const Map& map);

private:
    void tryMove(const sf::Vector2i& direction, const Map& map);
};
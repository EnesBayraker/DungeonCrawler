#pragma once

#include "Entity.h"

class Enemy : public Entity
{
public:
    explicit Enemy(const sf::Vector2i& gridPosition);
};
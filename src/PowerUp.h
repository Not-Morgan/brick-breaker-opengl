#pragma once

#include "gameObject.h"
#include <iostream>

const glm::vec2 POWER_UP_SIZE(60, 20);
const glm::vec2 POWER_UP_VELOCITY(0.0f, 150.0f);

enum powerUpType {
    speed,
    paddleSize,
    extraBall,
    passThrough,
};

class PowerUp : public GameObject {
public:
    powerUpType type;
    float duration;
    bool active;

    PowerUp(powerUpType type, glm::vec3 color, float duration, glm::vec2 position, Texture2D texture)
    : GameObject(position, POWER_UP_SIZE, texture, color, POWER_UP_VELOCITY), type(type), duration(duration), active()
    {}
};


#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "texture.h"
#include "SpriteRenderer.h"

// container object holding all relevant information
// that anything on the game will need
class GameObject
{
public:
    glm::vec2 position, size, velocity;
    glm::vec3 color;
    GLfloat rotation;
    bool isSolid;
    bool isDestroyed;

    Texture2D sprite;

    GameObject();
    GameObject(glm::vec2 pos, glm::vec2 size, Texture2D sprite, glm::vec3 color, glm::vec2 velocity = glm::vec2(0.0f, 0.0f));

    virtual void Draw(SpriteRenderer &renderer);
};


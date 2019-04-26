#pragma once

#include "gameObject.h"

class BallObject : public GameObject
{
public:
    GLfloat radius;
    bool stuck;

    BallObject();
    BallObject(glm::vec2 pos, GLfloat radius, glm::vec2 velocity, Texture2D sprite);

    void reset(glm::vec2 pos);
    glm::vec2 Move(GLfloat dt, GLuint window_width);
};



#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>

#include "SpriteRenderer.h"
#include "gameObject.h"
#include "resourceMgr.h"

class GameLevel
{
public:
    std::vector<GameObject> bricks;

    GameLevel() = default;

    void load(const char * filePath, GLuint levelWidth, GLuint levelHeight);
    void Draw(SpriteRenderer & renderer);
    GLboolean isCompleted();

private:
    void init(std::vector<std::vector<GLuint>> tileData, GLuint levelWidth, GLuint levelHeight);
};


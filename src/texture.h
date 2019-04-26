#pragma once
#include <GL/glew.h>

class Texture2D
{
public:
    GLuint ID;

    Texture2D();
    void bind() const;
};

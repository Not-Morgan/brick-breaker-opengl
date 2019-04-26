#include "texture.h"

Texture2D::Texture2D()
{

}

void Texture2D::bind() const
{
    glBindTexture(GL_TEXTURE_2D, ID);
}
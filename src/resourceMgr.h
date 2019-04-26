#pragma once

#include <GL/glew.h>
#include <map>
#include "shader.h"
#include "texture.h"

class ResourceManager
{
public:
    static Shader loadShader(const GLchar * vertexPath, const GLchar * fragmentPath, const GLchar * geometryPath, std::string name);
    static Shader getShader(std::string name);

    static Texture2D loadTexture(const GLchar *filepath, bool alpha, std::string name);
    static Texture2D getTexture(std::string name);

    // deallocates all resources
    static void clear();
private:
    static std::map<std::string, Shader> shaders;
    static std::map<std::string, Texture2D> textures;

    // private constructor so there are no objects
    ResourceManager() = default;

    static Shader loadShaderFromFile(const char * vertexPath, const char * fragmentPath, const char * geometryPath);
    static Texture2D loadTextureFromFile(const char * filePath, bool alpha);
    static std::string readFile(const char * filePath);
};

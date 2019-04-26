#include "resourceMgr.h"

#include <IL/il.h>
#include <IL/ilu.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

std::map<std::string, Texture2D>    ResourceManager::textures;
std::map<std::string, Shader>       ResourceManager::shaders;

Shader ResourceManager::loadShader(const char * vertexPath, const char * fragmentPath, const char * geometryPath, std::string name)
{
    shaders[name] = loadShaderFromFile(vertexPath, fragmentPath, geometryPath);
    return shaders[name];
}

Shader ResourceManager::getShader(std::string name)
{
    return shaders[name];
}

Texture2D ResourceManager::loadTexture(const GLchar *filepath, bool alpha, std::string name)
{
    Texture2D newTexture = loadTextureFromFile(filepath, alpha);
    textures[name] = newTexture;
    return textures[name];
}

Texture2D ResourceManager::getTexture(std::string name)
{
    return textures[name];
}

Shader ResourceManager::loadShaderFromFile(const char * vertexPath, const char * fragmentPath, const char * geometryPath) {
    std::string vCode = readFile(vertexPath);
    std::string fCode = readFile(fragmentPath);
    const char * vertexCode = vCode.c_str();
    const char * fragmentCode = fCode.c_str();

    Shader newShader = Shader();
    newShader.compile(vertexCode, fragmentCode, nullptr);

    return newShader;
}

void ResourceManager::clear()
{
    for (auto iter : shaders)
        glDeleteProgram(iter.second.ID);
    for (auto iter : textures)
        glDeleteTextures(1, &iter.second.ID);
}

Texture2D ResourceManager::loadTextureFromFile(const char * filePath, bool alpha) {
    ILuint imageID;
    GLuint textureID;
    ILboolean success;
    ILenum error;

    ilGenImages(1, &imageID);
    ilBindImage(imageID);

    success = ilLoadImage(filePath);

    if (success)
    {
        // if the image is flipped unflip it
//        ILinfo imageInfo;
//        iluGetImageInfo(&imageInfo);
//        if (imageInfo.Origin == IL_ORIGIN_UPPER_LEFT)
//        {
//            std::cout << "test" << std::endl;
//            iluFlipImage();
//        }

        // convert the image into the right format
        if (alpha)
        {
            success = ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);
        }
        else
        {
            success = ilConvertImage(IL_RGB, IL_UNSIGNED_BYTE);
        }

        if (!success)
        {
            error = ilGetError();
            std::cerr << "Image conversion failed: \n" << error << " - " << iluErrorString(error) << std::endl;
            exit(-1);
        }
        // make a new opengl texture
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);

        // set something
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        // set something else
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        // make the actual texture
        glTexImage2D(GL_TEXTURE_2D, 0, ilGetInteger(IL_IMAGE_FORMAT), ilGetInteger(IL_IMAGE_WIDTH),
                     ilGetInteger(IL_IMAGE_HEIGHT), 0, static_cast<GLenum>(ilGetInteger(IL_IMAGE_FORMAT)), GL_UNSIGNED_BYTE, ilGetData());
    }
    else
    {
        error = ilGetError();
        std::cerr << "Image loading failed: \n" << error << " - " << iluErrorString(error) << std::endl;
        exit(-1);
    }
    // delete the image as it is now in the texture
    ilDeleteImages(1, &imageID);

    Texture2D newTexture = Texture2D();
    newTexture.ID = textureID;

    return newTexture;
}

std::string ResourceManager::readFile(const char *filePath)
{
    std::string code;
    std::ifstream file;
    std::stringstream fileStream;

    try
    {
        file.open(filePath);
        // read file into stream
        fileStream << file.rdbuf();
        file.close();

        // convert into string
        code = fileStream.str();
    }
    catch(std::ifstream::failure& e)
    {
        std::cerr << "error failed to read shader file" << std::endl;
    }
    return code;
}
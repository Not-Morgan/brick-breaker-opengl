#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <fstream>

class Shader
{
public:
    unsigned int ID;

    // constructor reads from a file and builds the shader
    Shader();

    // sets the shader as the one being used
    void use();

    void compile(const GLchar *vertexSource, const GLchar *fragmentSource, const GLchar *geometrySource = nullptr);

    // set uniform values

    void setInt(const char * name, GLint value) const;
    void setFloat(const char * name, GLfloat value) const;
    void setVector2f(const char * name, const glm::vec2 & value) const;
    void setVector3f(const char * name, const glm::vec3 & value) const;
    void setVector4f(const char * name, const glm::vec4 & value) const;
    void setMatrix4(const char * name, const glm::mat4 & value) const;
};

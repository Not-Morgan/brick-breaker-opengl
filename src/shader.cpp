#include "shader.h"
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

Shader::Shader()
{

}

void Shader::use()
{
    glUseProgram(ID);
}

void Shader::compile(const GLchar *vertexSource, const GLchar *fragmentSource, const GLchar *geometrySource)
{
    GLuint vertex, fragment; //, geometry;
    GLint success;
    GLchar infoLog[1024];

    // vertex shader
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vertexSource, NULL);
    glCompileShader(vertex);
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertex, 512, NULL, infoLog);
        std::cerr << "error vertex shader compilation failed\n" << infoLog << std::endl;
    }

    // fragment shader
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fragmentSource, NULL);
    glCompileShader(fragment);
    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragment, 512, NULL, infoLog);
        std::cerr << "error fragment shader compilation failed\n" << infoLog << std::endl;
    }

    // geometry shader
    if (geometrySource != nullptr)
    {
        std::cout << "geometry shader does not work right now" << std::endl;
        exit(-1);
    }

    // shader program
    this->ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    glLinkProgram(ID);

    glGetProgramiv(ID, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(ID, 512, NULL, infoLog);
        std::cerr << "error shader program compilation failed\n" << infoLog << std::endl;
    }

    // delete shaders as they are now linked in the program
    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

void Shader::setInt(const char * name, GLint value) const
{ 
    glUniform1i(glGetUniformLocation(ID, name), value);
}
void Shader::setFloat(const char * name, GLfloat value) const
{ 
    glUniform1f(glGetUniformLocation(ID, name), value);
}
void Shader::setVector2f(const char *name, const glm::vec2 &value) const
{
    glUniform2f(glGetUniformLocation(ID, name), value.x, value.y);
}
void Shader::setVector3f(const char *name, const glm::vec3 &value) const
{
    glUniform3f(glGetUniformLocation(ID, name), value.x, value.y, value.z);
}
void Shader::setVector4f(const char *name, const glm::vec4 &value) const
{
    glUniform4f(glGetUniformLocation(ID, name), value.x, value.y, value.z, value.w);
}
void Shader::setMatrix4(const char *name, const glm::mat4 &matrix) const
{
    glUniformMatrix4fv(glGetUniformLocation(ID, name), 1, GL_FALSE, glm::value_ptr(matrix));
}

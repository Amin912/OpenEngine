#include "engine/Shader.h"

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

#include <fstream>
#include <sstream>
#include <stdexcept>

namespace oe {

namespace {

std::string readFile(const std::string& path)
{
    std::ifstream file(path);
    if (!file) {
        throw std::runtime_error("Could not open " + path);
    }
    std::stringstream text;
    text << file.rdbuf();
    return text.str();
}

unsigned int compile(GLenum type, const std::string& path)
{
    const std::string source = readFile(path);
    const char* text = source.c_str();
    const unsigned int shader = glCreateShader(type);
    glShaderSource(shader, 1, &text, nullptr);
    glCompileShader(shader);

    int ok = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        char log[1024];
        glGetShaderInfoLog(shader, sizeof(log), nullptr, log);
        glDeleteShader(shader);
        throw std::runtime_error("Could not compile " + path + ":\n" + log);
    }
    return shader;
}

}  // namespace

Shader::Shader(const std::string& vertexPath, const std::string& fragmentPath)
{
    const unsigned int vertex = compile(GL_VERTEX_SHADER, vertexPath);
    unsigned int fragment = 0;
    try {
        fragment = compile(GL_FRAGMENT_SHADER, fragmentPath);
    } catch (...) {
        glDeleteShader(vertex);
        throw;
    }

    id = glCreateProgram();
    glAttachShader(id, vertex);
    glAttachShader(id, fragment);
    glLinkProgram(id);
    glDeleteShader(vertex);
    glDeleteShader(fragment);

    int ok = 0;
    glGetProgramiv(id, GL_LINK_STATUS, &ok);
    if (!ok) {
        char log[1024];
        glGetProgramInfoLog(id, sizeof(log), nullptr, log);
        glDeleteProgram(id);
        throw std::runtime_error("Could not link " + vertexPath + " and " + fragmentPath + ":\n" + log);
    }
}

Shader::~Shader()
{
    glDeleteProgram(id);
}

void Shader::use() const
{
    glUseProgram(id);
}

int Shader::location(const char* name) const
{
    auto it = locations.find(name);
    if (it == locations.end()) {
        it = locations.emplace(name, glGetUniformLocation(id, name)).first;
    }
    return it->second;
}

void Shader::setInt(const char* name, int value) const
{
    glUniform1i(location(name), value);
}

void Shader::setFloat(const char* name, float value) const
{
    glUniform1f(location(name), value);
}

void Shader::setVec3(const char* name, const glm::vec3& value) const
{
    glUniform3fv(location(name), 1, glm::value_ptr(value));
}

void Shader::setMat4(const char* name, const glm::mat4& value) const
{
    glUniformMatrix4fv(location(name), 1, GL_FALSE, glm::value_ptr(value));
}

}  // namespace oe

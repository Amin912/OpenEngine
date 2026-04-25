#pragma once

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

#include <string>
#include <unordered_map>

namespace oe {

// A vertex and fragment shader linked into one program.
class Shader
{
public:
    // Reads, compiles and links the two files. Throws std::runtime_error
    // with the compiler's log if anything fails.
    Shader(const std::string& vertexPath, const std::string& fragmentPath);
    ~Shader();
    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;

    void use() const;
    void setInt(const char* name, int value) const;
    void setFloat(const char* name, float value) const;
    void setVec3(const char* name, const glm::vec3& value) const;
    void setMat4(const char* name, const glm::mat4& value) const;

private:
    int location(const char* name) const;

    unsigned int id = 0;
    // Looking a uniform up by name every frame is slow, so locations are cached.
    mutable std::unordered_map<std::string, int> locations;
};

}  // namespace oe

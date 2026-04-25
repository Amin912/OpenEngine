#pragma once

#include <glm/vec3.hpp>

#include <vector>

namespace oe {

// A 2D texture with mipmaps that repeats when its coordinates go past 1.
class Texture
{
public:
    // rgba holds width * height * 4 bytes, row by row from the top.
    Texture(int width, int height, const std::vector<unsigned char>& rgba);
    ~Texture();
    Texture(Texture&& other) noexcept;
    Texture& operator=(Texture&& other) noexcept;
    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;

    void bind(int unit) const;

    // Textures generated in code, so the demo needs no image files.
    static Texture checker(int size, int squares, const glm::vec3& a, const glm::vec3& b);
    static Texture panels(int size, const glm::vec3& face, const glm::vec3& edge);

private:
    unsigned int id = 0;
};

}  // namespace oe

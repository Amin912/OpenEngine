#include "engine/Texture.h"

#include <glad/glad.h>

#include <algorithm>
#include <cmath>
#include <utility>

namespace oe {

Texture::Texture(int width, int height, const std::vector<unsigned char>& rgba)
{
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8_ALPHA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, rgba.data());
    glGenerateMipmap(GL_TEXTURE_2D);
}

Texture::~Texture()
{
    glDeleteTextures(1, &id);
}

Texture::Texture(Texture&& other) noexcept : id(std::exchange(other.id, 0)) {}

Texture& Texture::operator=(Texture&& other) noexcept
{
    std::swap(id, other.id);
    return *this;
}

void Texture::bind(int unit) const
{
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, id);
}

namespace {

void put(std::vector<unsigned char>& pixels, int index, const glm::vec3& color)
{
    pixels[index * 4 + 0] = static_cast<unsigned char>(std::clamp(color.r, 0.0f, 1.0f) * 255.0f);
    pixels[index * 4 + 1] = static_cast<unsigned char>(std::clamp(color.g, 0.0f, 1.0f) * 255.0f);
    pixels[index * 4 + 2] = static_cast<unsigned char>(std::clamp(color.b, 0.0f, 1.0f) * 255.0f);
    pixels[index * 4 + 3] = 255;
}

}  // namespace

Texture Texture::checker(int size, int squares, const glm::vec3& a, const glm::vec3& b)
{
    std::vector<unsigned char> pixels(static_cast<size_t>(size) * size * 4);
    const int square = size / squares;
    for (int y = 0; y < size; ++y) {
        for (int x = 0; x < size; ++x) {
            const bool even = ((x / square) + (y / square)) % 2 == 0;
            put(pixels, y * size + x, even ? a : b);
        }
    }
    return Texture(size, size, pixels);
}

Texture Texture::panels(int size, const glm::vec3& face, const glm::vec3& edge)
{
    std::vector<unsigned char> pixels(static_cast<size_t>(size) * size * 4);
    const float border = size * 0.07f;
    for (int y = 0; y < size; ++y) {
        for (int x = 0; x < size; ++x) {
            // Distance to the nearest side, for a frame around each face.
            const float side = static_cast<float>(std::min(std::min(x, y), std::min(size - 1 - x, size - 1 - y)));
            // Two diagonal grooves across the face, and a little grain.
            const float groove = std::min(std::abs(x - y), std::abs(x + y - size)) / static_cast<float>(size);
            const float grain = 0.04f * std::sin(y * 0.9f + std::sin(x * 0.11f) * 3.0f);
            glm::vec3 color = face * (1.0f + grain);
            if (groove < 0.025f) {
                color *= 0.8f;
            }
            if (side < border) {
                color = edge * (1.0f + grain);
            }
            put(pixels, y * size + x, color);
        }
    }
    return Texture(size, size, pixels);
}

}  // namespace oe

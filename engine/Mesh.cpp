#include "engine/Mesh.h"

#include <glad/glad.h>
#include <glm/gtc/constants.hpp>

#include <cmath>
#include <cstddef>
#include <utility>

namespace oe {

Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices)
    : indexCount(static_cast<int>(indices.size()))
{
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(vertices.size() * sizeof(Vertex)), vertices.data(),
                 GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(indices.size() * sizeof(unsigned int)),
                 indices.data(), GL_STATIC_DRAW);

    // Matches the inputs of the vertex shaders: 0 position, 1 normal, 2 uv.
    const auto stride = static_cast<GLsizei>(sizeof(Vertex));
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<void*>(offsetof(Vertex, position)));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<void*>(offsetof(Vertex, normal)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<void*>(offsetof(Vertex, uv)));
    glEnableVertexAttribArray(2);
    glBindVertexArray(0);
}

Mesh::~Mesh()
{
    glDeleteBuffers(1, &ebo);
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
}

Mesh::Mesh(Mesh&& other) noexcept
    : vao(std::exchange(other.vao, 0)),
      vbo(std::exchange(other.vbo, 0)),
      ebo(std::exchange(other.ebo, 0)),
      indexCount(std::exchange(other.indexCount, 0))
{
}

Mesh& Mesh::operator=(Mesh&& other) noexcept
{
    std::swap(vao, other.vao);
    std::swap(vbo, other.vbo);
    std::swap(ebo, other.ebo);
    std::swap(indexCount, other.indexCount);
    return *this;
}

void Mesh::draw() const
{
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr);
}

Mesh Mesh::cube()
{
    // For each face: its normal and the two axes spanning it.
    struct Face {
        glm::vec3 normal, u, v;
    };
    const Face faces[6] = {
        {{0, 0, 1}, {1, 0, 0}, {0, 1, 0}},   {{0, 0, -1}, {-1, 0, 0}, {0, 1, 0}},
        {{1, 0, 0}, {0, 0, -1}, {0, 1, 0}},  {{-1, 0, 0}, {0, 0, 1}, {0, 1, 0}},
        {{0, 1, 0}, {1, 0, 0}, {0, 0, -1}},  {{0, -1, 0}, {1, 0, 0}, {0, 0, 1}},
    };
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    for (const Face& f : faces) {
        const auto base = static_cast<unsigned int>(vertices.size());
        const glm::vec3 centre = f.normal * 0.5f;
        vertices.push_back({centre - f.u * 0.5f - f.v * 0.5f, f.normal, {0, 0}});
        vertices.push_back({centre + f.u * 0.5f - f.v * 0.5f, f.normal, {1, 0}});
        vertices.push_back({centre + f.u * 0.5f + f.v * 0.5f, f.normal, {1, 1}});
        vertices.push_back({centre - f.u * 0.5f + f.v * 0.5f, f.normal, {0, 1}});
        indices.insert(indices.end(), {base, base + 1, base + 2, base, base + 2, base + 3});
    }
    return Mesh(vertices, indices);
}

Mesh Mesh::plane(float size, float tiles)
{
    const float h = size / 2;
    const glm::vec3 up(0, 1, 0);
    const std::vector<Vertex> vertices = {
        {{-h, 0, h}, up, {0, 0}},
        {{h, 0, h}, up, {tiles, 0}},
        {{h, 0, -h}, up, {tiles, tiles}},
        {{-h, 0, -h}, up, {0, tiles}},
    };
    return Mesh(vertices, {0, 1, 2, 0, 2, 3});
}

Mesh Mesh::sphere(int segments, int rings)
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    for (int r = 0; r <= rings; ++r) {
        const float v = static_cast<float>(r) / rings;
        const float polar = v * glm::pi<float>();
        for (int s = 0; s <= segments; ++s) {
            const float u = static_cast<float>(s) / segments;
            const float azimuth = u * glm::two_pi<float>();
            const glm::vec3 p(std::sin(polar) * std::cos(azimuth), std::cos(polar), std::sin(polar) * std::sin(azimuth));
            vertices.push_back({p * 0.5f, p, {u, 1.0f - v}});
        }
    }
    const auto columns = static_cast<unsigned int>(segments + 1);
    for (unsigned int r = 0; r < static_cast<unsigned int>(rings); ++r) {
        for (unsigned int s = 0; s < static_cast<unsigned int>(segments); ++s) {
            const unsigned int a = r * columns + s;
            const unsigned int b = a + columns;
            indices.insert(indices.end(), {a, b, a + 1, a + 1, b, b + 1});
        }
    }
    return Mesh(vertices, indices);
}

}  // namespace oe

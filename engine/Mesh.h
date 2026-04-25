#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include <vector>

namespace oe {

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 uv;
};

// Vertices and triangle indices uploaded to the GPU.
class Mesh
{
public:
    Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);
    ~Mesh();
    Mesh(Mesh&& other) noexcept;
    Mesh& operator=(Mesh&& other) noexcept;
    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;

    void draw() const;

    // A unit cube centred on the origin, with flat normals per face.
    static Mesh cube();
    // A flat square on the XZ plane, with the texture repeated `tiles` times.
    static Mesh plane(float size, float tiles);
    // A unit sphere built from rings of quads.
    static Mesh sphere(int segments, int rings);

private:
    unsigned int vao = 0;
    unsigned int vbo = 0;
    unsigned int ebo = 0;
    int indexCount = 0;
};

}  // namespace oe

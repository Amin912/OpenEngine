// OpenEngine demo: a ring of spinning crates over a tiled floor, with a
// free-flying camera.
//
//   Click         capture the mouse to look around (Esc releases it)
//   W A S D       move          Q / E    down / up
//   Scroll        zoom          Up/Down  blend between the two crate textures
//   Esc           quit (when the mouse is not captured)

#include "engine/Camera.h"
#include "engine/Mesh.h"
#include "engine/Shader.h"
#include "engine/Texture.h"
#include "engine/Window.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <algorithm>
#include <cmath>
#include <exception>
#include <filesystem>
#include <iostream>
#include <vector>

namespace {

void handleInput(oe::Window& window, oe::Camera& camera, float& blend, float dt)
{
    if (window.wasKeyPressed(GLFW_KEY_ESCAPE)) {
        if (window.isMouseCaptured()) {
            window.captureMouse(false);
        } else {
            window.close();
        }
    }
    if (!window.isMouseCaptured() && window.isMouseButtonDown(GLFW_MOUSE_BUTTON_LEFT)) {
        window.captureMouse(true);
    }

    glm::vec3 direction(0.0f);
    direction.z += window.isKeyDown(GLFW_KEY_W) ? 1.0f : 0.0f;
    direction.z -= window.isKeyDown(GLFW_KEY_S) ? 1.0f : 0.0f;
    direction.x += window.isKeyDown(GLFW_KEY_D) ? 1.0f : 0.0f;
    direction.x -= window.isKeyDown(GLFW_KEY_A) ? 1.0f : 0.0f;
    direction.y += window.isKeyDown(GLFW_KEY_E) ? 1.0f : 0.0f;
    direction.y -= window.isKeyDown(GLFW_KEY_Q) ? 1.0f : 0.0f;
    camera.move(direction, dt);
    camera.look(window.mouseMovement());
    camera.zoom(window.scrollMovement());

    if (window.isKeyDown(GLFW_KEY_UP)) {
        blend = std::min(blend + dt, 1.0f);
    }
    if (window.isKeyDown(GLFW_KEY_DOWN)) {
        blend = std::max(blend - dt, 0.0f);
    }
}

}  // namespace

int main(int, char** argv)
{
    try {
        // Shaders are copied next to the executable by the build.
        const std::filesystem::path shaders = std::filesystem::absolute(argv[0]).parent_path() / "shaders";

        oe::Window window(1280, 720, "OpenEngine");
        oe::Shader shader((shaders / "basic.vert").string(), (shaders / "basic.frag").string());

        const oe::Mesh cube = oe::Mesh::cube();
        const oe::Mesh floor = oe::Mesh::plane(60.0f, 30.0f);
        const oe::Texture wood = oe::Texture::panels(256, {0.62f, 0.42f, 0.24f}, {0.33f, 0.21f, 0.11f});
        const oe::Texture steel = oe::Texture::panels(256, {0.45f, 0.52f, 0.60f}, {0.22f, 0.26f, 0.31f});
        const oe::Texture tiles = oe::Texture::checker(256, 2, {0.80f, 0.82f, 0.86f}, {0.56f, 0.59f, 0.65f});

        // Ten crates on a ring, each at its own height.
        std::vector<glm::vec3> crates;
        for (int i = 0; i < 10; ++i) {
            const float angle = i * glm::two_pi<float>() / 10.0f;
            crates.emplace_back(4.5f * std::cos(angle), 1.0f + 0.6f * std::sin(i * 1.7f), 4.5f * std::sin(angle));
        }

        oe::Camera camera;
        float blend = 0.0f;
        double last = window.time();

        while (window.isOpen()) {
            window.pollEvents();
            const double now = window.time();
            const float dt = static_cast<float>(now - last);
            last = now;
            handleInput(window, camera, blend, dt);

            glClearColor(0.62f, 0.74f, 0.86f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            shader.use();
            shader.setMat4("view", camera.view());
            shader.setMat4("projection", camera.projection(window.aspect()));
            shader.setInt("textureA", 0);
            shader.setInt("textureB", 1);

            tiles.bind(0);
            tiles.bind(1);
            shader.setFloat("blend", 0.0f);
            shader.setMat4("model", glm::mat4(1.0f));
            floor.draw();

            wood.bind(0);
            steel.bind(1);
            shader.setFloat("blend", blend);
            for (std::size_t i = 0; i < crates.size(); ++i) {
                glm::mat4 model = glm::translate(glm::mat4(1.0f), crates[i]);
                model = glm::rotate(model, static_cast<float>(now) * 0.6f + i, glm::normalize(glm::vec3(1.0f, 0.3f, 0.5f)));
                shader.setMat4("model", model);
                cube.draw();
            }

            window.swapBuffers();
        }
    } catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
        return 1;
    }
    return 0;
}

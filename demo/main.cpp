// OpenEngine demo: a ring of spinning crates over a tiled floor, with a
// free-flying camera.
//
//   Click         capture the mouse to look around (Esc releases it)
//   W A S D       move          Q / E    down / up
//   Scroll        zoom          Up/Down  blend between the two crate textures
//   F12           save a screenshot next to the executable
//   Esc           quit (when the mouse is not captured)
//
// demo --screenshot out.bmp renders one frame without showing a window.

#include "engine/Camera.h"
#include "engine/Mesh.h"
#include "engine/Screenshot.h"
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
#include <string>
#include <vector>

namespace {

constexpr int Width = 1280;
constexpr int Height = 720;
const glm::vec3 Sky(0.62f, 0.74f, 0.86f);

// Everything in the world, and how to draw it at a given time.
struct Scene {
    explicit Scene(const std::filesystem::path& shaders)
        : shader((shaders / "lit.vert").string(), (shaders / "lit.frag").string()),
          cube(oe::Mesh::cube()),
          floor(oe::Mesh::plane(60.0f, 30.0f)),
          wood(oe::Texture::panels(256, {0.62f, 0.42f, 0.24f}, {0.33f, 0.21f, 0.11f})),
          steel(oe::Texture::panels(256, {0.45f, 0.52f, 0.60f}, {0.22f, 0.26f, 0.31f})),
          tiles(oe::Texture::checker(256, 2, {0.80f, 0.82f, 0.86f}, {0.56f, 0.59f, 0.65f}))
    {
        // Ten crates on a ring, each at its own height.
        for (int i = 0; i < 10; ++i) {
            const float angle = i * glm::two_pi<float>() / 10.0f;
            crates.emplace_back(4.5f * std::cos(angle), 1.0f + 0.6f * std::sin(i * 1.7f), 4.5f * std::sin(angle));
        }
    }

    void draw(const oe::Camera& camera, float aspect, float time) const
    {
        glClearColor(Sky.r, Sky.g, Sky.b, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.use();
        shader.setMat4("view", camera.view());
        shader.setMat4("projection", camera.projection(aspect));
        shader.setInt("textureA", 0);
        shader.setInt("textureB", 1);
        shader.setVec3("cameraPosition", camera.position);
        shader.setVec3("lightDirection", glm::normalize(glm::vec3(-0.4f, -1.0f, -0.3f)));
        shader.setVec3("skyColor", Sky);
        shader.setVec3("tint", glm::vec3(1.0f));

        tiles.bind(0);
        tiles.bind(1);
        shader.setFloat("blend", 0.0f);
        shader.setFloat("shininess", 8.0f);
        shader.setMat4("model", glm::mat4(1.0f));
        floor.draw();

        wood.bind(0);
        steel.bind(1);
        shader.setFloat("blend", blend);
        shader.setFloat("shininess", 24.0f);
        for (std::size_t i = 0; i < crates.size(); ++i) {
            glm::mat4 model = glm::translate(glm::mat4(1.0f), crates[i]);
            model = glm::rotate(model, time * 0.6f + static_cast<float>(i), glm::normalize(glm::vec3(1.0f, 0.3f, 0.5f)));
            shader.setMat4("model", model);
            cube.draw();
        }
    }

    oe::Shader shader;
    oe::Mesh cube;
    oe::Mesh floor;
    oe::Texture wood;
    oe::Texture steel;
    oe::Texture tiles;
    std::vector<glm::vec3> crates;
    float blend = 0.0f;
};

void handleInput(oe::Window& window, oe::Camera& camera, Scene& scene, float dt)
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
        scene.blend = std::min(scene.blend + dt, 1.0f);
    }
    if (window.isKeyDown(GLFW_KEY_DOWN)) {
        scene.blend = std::max(scene.blend - dt, 0.0f);
    }
}

int run(const std::filesystem::path& folder, const std::string& screenshot)
{
    const bool offscreen = !screenshot.empty();
    oe::Window window(Width, Height, "OpenEngine", !offscreen);
    Scene scene(folder / "shaders");
    oe::Camera camera;

    if (offscreen) {
        const oe::OffscreenTarget target(Width, Height);
        target.bind();
        scene.draw(camera, static_cast<float>(Width) / Height, 2.0f);
        target.save(screenshot);
        return 0;
    }

    double last = window.time();
    int shots = 0;
    while (window.isOpen()) {
        window.pollEvents();
        const double now = window.time();
        const float dt = static_cast<float>(now - last);
        last = now;
        handleInput(window, camera, scene, dt);

        scene.draw(camera, window.aspect(), static_cast<float>(now));
        if (window.wasKeyPressed(GLFW_KEY_F12)) {
            glReadBuffer(GL_BACK);
            oe::saveFramebuffer((folder / ("screenshot-" + std::to_string(++shots) + ".bmp")).string(),
                                window.width(), window.height());
        }
        window.swapBuffers();
    }
    return 0;
}

}  // namespace

int main(int argc, char** argv)
{
    std::string screenshot;
    for (int i = 1; i < argc; ++i) {
        if (std::string(argv[i]) == "--screenshot" && i + 1 < argc) {
            screenshot = argv[++i];
        }
    }
    try {
        // Shaders are copied next to the executable by the build.
        return run(std::filesystem::absolute(argv[0]).parent_path(), screenshot);
    } catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
        return 1;
    }
}

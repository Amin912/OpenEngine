#pragma once

#include <glm/vec2.hpp>

#include <array>
#include <string>

struct GLFWwindow;

namespace oe {

// A window with an OpenGL 3.3 core context, plus the keyboard and mouse
// state for the current frame. There is one per program.
class Window
{
public:
    // Throws std::runtime_error if GLFW, the window or OpenGL fail to start.
    Window(int width, int height, const std::string& title, bool visible = true);
    ~Window();
    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

    bool isOpen() const;
    void close();
    void swapBuffers();
    // Fetches new input. Mouse movement, scrolling and key presses are
    // counted from one call to the next.
    void pollEvents();
    double time() const;

    bool isKeyDown(int key) const;
    bool wasKeyPressed(int key) const;
    bool isMouseButtonDown(int button) const;
    glm::vec2 mouseMovement() const { return mouseMove; }
    float scrollMovement() const { return scroll; }

    // While captured the cursor is hidden and the mouse moves the camera.
    void captureMouse(bool capture);
    bool isMouseCaptured() const { return mouseCaptured; }

    // Framebuffer size in pixels, which can differ from the window size on
    // high-DPI screens.
    int width() const;
    int height() const;
    float aspect() const;

private:
    static Window& from(GLFWwindow* window);

    GLFWwindow* window = nullptr;
    std::array<bool, 512> pressed{};
    std::array<bool, 512> pressedNow{};
    glm::vec2 lastMouse{0.0f};
    glm::vec2 mouseMove{0.0f};
    glm::vec2 pendingMove{0.0f};
    float scroll = 0.0f;
    float pendingScroll = 0.0f;
    bool firstMouse = true;
    bool mouseCaptured = false;
};

}  // namespace oe

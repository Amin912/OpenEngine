#include "engine/Window.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <cstdio>
#include <stdexcept>

namespace oe {

namespace {

void reportError(int code, const char* description)
{
    std::fprintf(stderr, "GLFW error %d: %s\n", code, description);
}

}  // namespace

Window::Window(int width, int height, const std::string& title, bool visible)
{
    glfwSetErrorCallback(reportError);
    if (!glfwInit()) {
        throw std::runtime_error("Could not start GLFW");
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);  // required on macOS
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_VISIBLE, visible ? GLFW_TRUE : GLFW_FALSE);

    window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        throw std::runtime_error("Could not create an OpenGL 3.3 window");
    }
    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
        glfwDestroyWindow(window);
        glfwTerminate();
        throw std::runtime_error("Could not load the OpenGL functions");
    }
    glfwSwapInterval(1);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);

    glfwSetWindowUserPointer(window, this);
    glfwSetFramebufferSizeCallback(window, [](GLFWwindow*, int w, int h) { glViewport(0, 0, w, h); });
    glfwSetKeyCallback(window, [](GLFWwindow* w, int key, int, int action, int) {
        if (key >= 0 && key < 512 && action == GLFW_PRESS) {
            from(w).pressed[key] = true;
        }
    });
    glfwSetCursorPosCallback(window, [](GLFWwindow* w, double x, double y) {
        Window& self = from(w);
        const glm::vec2 position(static_cast<float>(x), static_cast<float>(y));
        if (self.firstMouse) {
            self.lastMouse = position;
            self.firstMouse = false;
        }
        if (self.mouseCaptured) {
            self.pendingMove += position - self.lastMouse;
        }
        self.lastMouse = position;
    });
    glfwSetScrollCallback(window, [](GLFWwindow* w, double, double y) {
        from(w).pendingScroll += static_cast<float>(y);
    });
}

Window::~Window()
{
    glfwDestroyWindow(window);
    glfwTerminate();
}

Window& Window::from(GLFWwindow* window)
{
    return *static_cast<Window*>(glfwGetWindowUserPointer(window));
}

bool Window::isOpen() const
{
    return !glfwWindowShouldClose(window);
}

void Window::close()
{
    glfwSetWindowShouldClose(window, GLFW_TRUE);
}

void Window::swapBuffers()
{
    glfwSwapBuffers(window);
}

void Window::pollEvents()
{
    pressed.fill(false);
    pendingMove = glm::vec2(0.0f);
    pendingScroll = 0.0f;
    glfwPollEvents();
    pressedNow = pressed;
    mouseMove = pendingMove;
    scroll = pendingScroll;
}

double Window::time() const
{
    return glfwGetTime();
}

bool Window::isKeyDown(int key) const
{
    return glfwGetKey(window, key) == GLFW_PRESS;
}

bool Window::wasKeyPressed(int key) const
{
    return key >= 0 && key < 512 && pressedNow[key];
}

bool Window::isMouseButtonDown(int button) const
{
    return glfwGetMouseButton(window, button) == GLFW_PRESS;
}

void Window::captureMouse(bool capture)
{
    mouseCaptured = capture;
    firstMouse = true;
    glfwSetInputMode(window, GLFW_CURSOR, capture ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
}

int Window::width() const
{
    int w = 0;
    glfwGetFramebufferSize(window, &w, nullptr);
    return w;
}

int Window::height() const
{
    int h = 0;
    glfwGetFramebufferSize(window, nullptr, &h);
    return h;
}

float Window::aspect() const
{
    const int h = height();
    return h > 0 ? static_cast<float>(width()) / static_cast<float>(h) : 1.0f;
}

}  // namespace oe

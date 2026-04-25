#include "engine/Camera.h"

#include <glm/geometric.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/trigonometric.hpp>

#include <algorithm>
#include <cmath>

namespace oe {

namespace {

const glm::vec3 WorldUp(0.0f, 1.0f, 0.0f);

}  // namespace

glm::vec3 Camera::front() const
{
    const float y = glm::radians(yaw);
    const float p = glm::radians(pitch);
    return glm::normalize(glm::vec3(std::cos(y) * std::cos(p), std::sin(p), std::sin(y) * std::cos(p)));
}

glm::vec3 Camera::right() const
{
    return glm::normalize(glm::cross(front(), WorldUp));
}

void Camera::move(const glm::vec3& direction, float dt)
{
    if (glm::length(direction) == 0.0f) {
        return;
    }
    // Forward follows the view but stays level, so looking down does not
    // make you sink into the floor.
    const glm::vec3 level = glm::normalize(glm::vec3(front().x, 0.0f, front().z));
    const glm::vec3 step = right() * direction.x + WorldUp * direction.y + level * direction.z;
    position += glm::normalize(step) * speed * dt;
}

void Camera::look(const glm::vec2& mouseMovement)
{
    yaw += mouseMovement.x * sensitivity;
    // Stop just short of straight up or down, where the view would flip.
    pitch = std::clamp(pitch - mouseMovement.y * sensitivity, -89.0f, 89.0f);
}

void Camera::zoom(float scroll)
{
    fieldOfView = std::clamp(fieldOfView - scroll * 2.0f, 20.0f, 90.0f);
}

glm::mat4 Camera::view() const
{
    return glm::lookAt(position, position + front(), WorldUp);
}

glm::mat4 Camera::projection(float aspect) const
{
    return glm::perspective(glm::radians(fieldOfView), aspect, 0.1f, 200.0f);
}

}  // namespace oe

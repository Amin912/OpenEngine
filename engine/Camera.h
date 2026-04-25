#pragma once

#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

namespace oe {

// A free-flying first-person camera. Yaw and pitch are in degrees; a yaw of
// -90 looks down the negative Z axis.
class Camera
{
public:
    glm::vec3 position{0.0f, 2.0f, 8.0f};
    float yaw = -90.0f;
    float pitch = -12.0f;
    float fieldOfView = 60.0f;
    float speed = 5.0f;          // units per second
    float sensitivity = 0.1f;    // degrees per pixel of mouse movement

    // `direction` is relative to the camera: x right, y up, z forward.
    void move(const glm::vec3& direction, float dt);
    void look(const glm::vec2& mouseMovement);
    void zoom(float scroll);

    glm::vec3 front() const;
    glm::vec3 right() const;
    glm::mat4 view() const;
    glm::mat4 projection(float aspect) const;
};

}  // namespace oe

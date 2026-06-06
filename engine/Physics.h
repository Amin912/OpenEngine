#pragma once

#include <glm/vec3.hpp>

#include <vector>

namespace oe {

struct Ball {
    glm::vec3 position{0.0f};
    glm::vec3 velocity{0.0f};
    float radius = 0.5f;
    // Heavier balls push lighter ones around more. Mass grows with volume.
    float mass() const { return radius * radius * radius; }
};

// Balls under gravity that bounce off the ground and off each other.
//
// The simulation always advances in steps of the same length, whatever the
// frame rate, so it behaves the same on a fast and a slow machine; update()
// keeps the time left over between frames for the next call.
class PhysicsWorld
{
public:
    static constexpr float TimeStep = 1.0f / 120.0f;

    glm::vec3 gravity{0.0f, -9.81f, 0.0f};
    float restitution = 0.6f;  // share of the speed kept after a bounce
    float rollingFriction = 0.6f;  // horizontal speed lost per second on the ground
    float groundHeight = 0.0f;
    std::vector<Ball> balls;

    void update(float dt);
    void step();

private:
    void collideWithGround(Ball& ball) const;
    void collide(Ball& a, Ball& b) const;

    float pending = 0.0f;
};

}  // namespace oe

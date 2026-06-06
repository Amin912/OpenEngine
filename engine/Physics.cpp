#include "engine/Physics.h"

#include <glm/geometric.hpp>

#include <algorithm>
#include <cmath>

namespace oe {

namespace {

// Below this speed a bounce is not worth simulating: the ball just stops.
constexpr float RestSpeed = 0.4f;

}  // namespace

void PhysicsWorld::update(float dt)
{
    // After a long pause (a breakpoint, dragging the window) catch up at most
    // a quarter of a second rather than simulating a huge backlog at once.
    pending += std::min(dt, 0.25f);
    // A little slack for rounding: frame times that add up to exactly N
    // steps can fall a hair short in floating point, which would drop a step.
    constexpr float Slack = TimeStep * 1e-3f;
    while (pending + Slack >= TimeStep) {
        step();
        pending -= TimeStep;
    }
}

void PhysicsWorld::step()
{
    // Semi-implicit Euler: update the velocity first, then move with it.
    for (Ball& ball : balls) {
        ball.velocity += gravity * TimeStep;
        ball.position += ball.velocity * TimeStep;
        collideWithGround(ball);
    }
    for (std::size_t i = 0; i < balls.size(); ++i) {
        for (std::size_t j = i + 1; j < balls.size(); ++j) {
            collide(balls[i], balls[j]);
        }
    }
}

void PhysicsWorld::collideWithGround(Ball& ball) const
{
    const float lowest = groundHeight + ball.radius;
    if (ball.position.y > lowest) {
        return;
    }
    ball.position.y = lowest;
    if (ball.velocity.y < 0.0f) {
        ball.velocity.y = -ball.velocity.y * restitution;
        if (ball.velocity.y < RestSpeed) {
            ball.velocity.y = 0.0f;
        }
    }
    const float keep = std::max(0.0f, 1.0f - rollingFriction * TimeStep);
    ball.velocity.x *= keep;
    ball.velocity.z *= keep;
}

void PhysicsWorld::collide(Ball& a, Ball& b) const
{
    const glm::vec3 offset = b.position - a.position;
    const float distance = glm::length(offset);
    const float touching = a.radius + b.radius;
    if (distance >= touching || distance == 0.0f) {
        return;
    }
    const glm::vec3 normal = offset / distance;
    const float inverseA = 1.0f / a.mass();
    const float inverseB = 1.0f / b.mass();

    // Push them apart so they no longer overlap, the lighter one further.
    const float overlap = touching - distance;
    a.position -= normal * (overlap * inverseA / (inverseA + inverseB));
    b.position += normal * (overlap * inverseB / (inverseA + inverseB));

    // If they are moving towards each other, exchange an impulse along the
    // line between their centres.
    const float approach = glm::dot(b.velocity - a.velocity, normal);
    if (approach < 0.0f) {
        const float impulse = -(1.0f + restitution) * approach / (inverseA + inverseB);
        a.velocity -= normal * (impulse * inverseA);
        b.velocity += normal * (impulse * inverseB);
    }
}

}  // namespace oe

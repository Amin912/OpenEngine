#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include "engine/Physics.h"

#include <glm/geometric.hpp>

#include <algorithm>

using oe::Ball;
using oe::PhysicsWorld;

namespace {

void run(PhysicsWorld& world, float seconds)
{
    const int steps = static_cast<int>(seconds / PhysicsWorld::TimeStep);
    for (int i = 0; i < steps; ++i) {
        world.step();
    }
}

}  // namespace

TEST_CASE("a dropped ball comes to rest on the ground")
{
    PhysicsWorld world;
    world.balls.push_back({{0.0f, 5.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, 0.5f});
    run(world, 10.0f);
    const Ball& ball = world.balls[0];
    CHECK(ball.position.y == doctest::Approx(0.5f));
    CHECK(glm::length(ball.velocity) == doctest::Approx(0.0f));
}

TEST_CASE("each bounce reaches a lower height")
{
    // Height goes with speed squared, so a bounce keeping 60% of the speed
    // should climb back to about 36% of the drop height.
    PhysicsWorld world;
    world.balls.push_back({{0.0f, 4.5f, 0.0f}, {0.0f, 0.0f, 0.0f}, 0.5f});
    bool landed = false;
    float peak = 0.0f;
    for (int i = 0; i < 600; ++i) {
        world.step();
        const Ball& ball = world.balls[0];
        if (ball.position.y <= 0.5f) {
            landed = true;
        }
        if (landed) {
            peak = std::max(peak, ball.position.y - 0.5f);
        }
        if (landed && ball.velocity.y < 0.0f && ball.position.y > 0.5f && peak > 0.0f) {
            break;  // past the top of the first bounce
        }
    }
    CHECK(peak / 4.0f == doctest::Approx(0.36f).epsilon(0.1));
}

TEST_CASE("update splits time into fixed steps and keeps the remainder")
{
    PhysicsWorld a;
    PhysicsWorld b;
    a.gravity = b.gravity = {0.0f, 0.0f, 0.0f};
    a.balls.push_back({{0.0f, 2.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, 0.5f});
    b.balls = a.balls;

    // Three frames of 1/90 s add up to four steps of 1/120 s.
    for (int i = 0; i < 3; ++i) {
        a.update(1.0f / 90.0f);
    }
    for (int i = 0; i < 4; ++i) {
        b.step();
    }
    CHECK(a.balls[0].position.x == doctest::Approx(b.balls[0].position.x));
}

TEST_CASE("equal balls in a perfectly elastic head-on collision swap speeds")
{
    PhysicsWorld world;
    world.gravity = {0.0f, 0.0f, 0.0f};
    world.restitution = 1.0f;
    world.balls.push_back({{-1.0f, 2.0f, 0.0f}, {2.0f, 0.0f, 0.0f}, 0.5f});
    world.balls.push_back({{1.0f, 2.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}, 0.5f});
    run(world, 1.0f);
    CHECK(world.balls[0].velocity.x == doctest::Approx(-1.0f));
    CHECK(world.balls[1].velocity.x == doctest::Approx(2.0f));
}

TEST_CASE("collisions keep momentum and leave no overlap")
{
    PhysicsWorld world;
    world.gravity = {0.0f, 0.0f, 0.0f};
    world.balls.push_back({{-2.0f, 2.0f, 0.1f}, {3.0f, 0.0f, 0.0f}, 0.5f});
    world.balls.push_back({{1.0f, 2.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, 0.8f});
    const auto momentum = [&] {
        glm::vec3 total(0.0f);
        for (const Ball& b : world.balls) {
            total += b.velocity * b.mass();
        }
        return total;
    };
    const glm::vec3 before = momentum();
    run(world, 2.0f);
    const glm::vec3 after = momentum();
    CHECK(after.x == doctest::Approx(before.x));
    CHECK(after.z == doctest::Approx(before.z).epsilon(0.001));
    const float gap = glm::length(world.balls[1].position - world.balls[0].position);
    CHECK(gap >= doctest::Approx(1.3f));
}

#include "Body.h"
#include "Vec2.h"

#include <gtest/gtest.h>

#include <array>

using namespace Math;
using namespace Physics;

struct TestBodyFixtureConstructor : public ::testing::TestWithParam<std::array<Vec2F, 3>> {};

INSTANTIATE_TEST_SUITE_P(Body, TestBodyFixtureConstructor, testing::Values(
	std::array<Vec2F, 3>{
		Vec2F(-5.0f, 2.0f), // position
		Vec2F(0.4f, 1.0f), // velocity
		Vec2F(1.4f, 0.24f) // force
	},
	std::array<Vec2F, 3>{
		Vec2F(0.0f, 0.0f), // position
		Vec2F(0.0f, 0.0f), // velocity
		Vec2F(0.0f, 0.0f) // force
	}
));

TEST_P(TestBodyFixtureConstructor, Constructor)
{
	auto params = GetParam();
	Vec2F position = params[0];
	Vec2F velocity = params[1];
	Vec2F force = params[2];
	float mass = 10.f;

	Body body(position, velocity);

	body.SetMass(mass);
    body.AddForce(force * mass);

	EXPECT_EQ(body.Position(), position);
	EXPECT_EQ(body.Velocity(), velocity);
	EXPECT_EQ(body.Force(), force * mass);
}
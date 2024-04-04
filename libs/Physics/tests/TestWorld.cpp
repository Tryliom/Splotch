#include "World.h"
#include "Body.h"
#include "Exception.h"

#include <gtest/gtest.h>

#include <array>

using namespace Physics;
using namespace Math;

struct TestWorldFixtureTime : public ::testing::TestWithParam<std::pair<std::array<Vec2F, 4>, std::pair<float, float>>> {};

INSTANTIATE_TEST_SUITE_P(World, TestWorldFixtureTime, testing::Values(
	std::make_pair(std::array<Vec2F, 4>{ Vec2F(0, 0), Vec2F(1, 1), Vec2F(1, 1), Vec2F(3.f, 0.34f)}, std::make_pair(1.3f, 1.f)),
	std::make_pair(std::array<Vec2F, 4>{ Vec2F(0.3f, 1.3f), Vec2F(0.634f, 1.f), Vec2F(0.6f, 1.6f) }, std::make_pair(1.9f, 20.0f)),
	std::make_pair(std::array<Vec2F, 4>{ Vec2F(0.3f, 1.3f), Vec2F(0.634f, 1.f), Vec2F(0.6f, 1.6f) }, std::make_pair(0.3244f, 100.4f)),
	std::make_pair(std::array<Vec2F, 4>{ Vec2F(0.3f, 1.3f), Vec2F(0.634f, 1.f), Vec2F(0.6f, 1.6f) }, std::make_pair(0.94f, 0.3244f))
));

enum class Interaction
{
	None, Enter, Exit, Stay
};

class TestContactListener : public ContactListener
{
public:
	TestContactListener(Interaction& interactionId, int& interactionCount) noexcept
		: _interactionId(interactionId), _interactionCount(interactionCount) {}

private:
	Interaction& _interactionId;
	int& _interactionCount;

public:
	void OnTriggerEnter(ColliderRef colliderRef, ColliderRef otherColliderRef) noexcept override
	{
		_interactionId = Interaction::Enter;
		_interactionCount++;
	}

	void OnTriggerExit(ColliderRef colliderRef, ColliderRef otherColliderRef) noexcept override
	{
		_interactionId = Interaction::Exit;
		_interactionCount++;
	}

	void OnTriggerStay(ColliderRef colliderRef, ColliderRef otherColliderRef) noexcept override
	{
		_interactionId = Interaction::Stay;
		_interactionCount++;
	}

    void OnCollisionEnter(ColliderRef colliderRef, ColliderRef otherColliderRef) noexcept override
    {
        _interactionId = Interaction::Enter;
        _interactionCount++;
    }

    void OnCollisionExit(ColliderRef colliderRef, ColliderRef otherColliderRef) noexcept override
    {
        _interactionId = Interaction::Exit;
        _interactionCount++;
    }

    void OnCollisionStay(ColliderRef colliderRef, ColliderRef otherColliderRef) noexcept override
    {
        _interactionId = Interaction::Stay;
        _interactionCount++;
    }
};

TEST(World, CreateBody)
{
    HeapAllocator allocator;
	World world;

	auto body = world.CreateBody();

	EXPECT_EQ(body.Index, 0);
	EXPECT_EQ(body.Generation, 0);
	EXPECT_TRUE(world.GetBody(body).IsEnabled());

	world.DestroyBody(body);

	body = world.CreateBody();

	EXPECT_EQ(body.Index, 0);
	EXPECT_EQ(body.Generation, 1);
	EXPECT_TRUE(world.GetBody(body).IsEnabled());

	auto body2 = world.CreateBody();

	EXPECT_EQ(body2.Index, 1);
	EXPECT_EQ(body2.Generation, 0);
	EXPECT_TRUE(world.GetBody(body2).IsEnabled());

	world.DestroyBody(body);

	EXPECT_THROW(world.GetBody(body), InvalidBodyRefException);
}

TEST(World, Collider)
{
    HeapAllocator allocator;
    World world;

	auto bodyRef = world.CreateBody();
	auto colliderRef = world.CreateCollider(bodyRef);
	auto& collider = world.GetCollider(colliderRef);

	EXPECT_FALSE(collider.IsEnabled());

	collider.SetCircle(CircleF({0.1f, 0.1f}, 1.f));

	EXPECT_EQ(colliderRef.Index, 0);
	EXPECT_EQ(colliderRef.Generation, 0);
	EXPECT_EQ(collider.GetBodyRef(), bodyRef);
	EXPECT_TRUE(collider.IsEnabled());
	EXPECT_FALSE(collider.IsTrigger());
	EXPECT_EQ(collider.GetRestitution(), 0.f);

	world.DestroyCollider(colliderRef);

	EXPECT_THROW(world.GetCollider(colliderRef), InvalidColliderRefException);

	colliderRef = world.CreateCollider(bodyRef);
	collider = world.GetCollider(colliderRef);

	collider.SetCircle(CircleF({0.1f, 0.1f}, 1.f));

	EXPECT_EQ(colliderRef.Index, 0);
	EXPECT_EQ(colliderRef.Generation, 1);
	EXPECT_EQ(collider.GetBodyRef(), bodyRef);
	EXPECT_TRUE(collider.IsEnabled());

	world.DestroyBody(bodyRef);

	EXPECT_THROW(world.GetCollider(colliderRef), InvalidColliderRefException);
}

TEST_P(TestWorldFixtureTime, Update)
{
    HeapAllocator allocator;
    World world;

	auto pair = GetParam();
	auto deltaTime = pair.second.first;
	auto bodyRef = world.CreateBody();
	auto& body = world.GetBody(bodyRef);

	body.SetPosition(pair.first[0]);
	body.SetVelocity(pair.first[1]);
	body.SetMass(pair.second.second);
    body.AddForce(pair.first[3] * body.Mass());

	auto velocity = body.Velocity() + body.Force() * deltaTime;
	auto position = body.Position() + velocity * deltaTime;

	world.Update(deltaTime);

	EXPECT_FLOAT_EQ(body.Position().X, position.X);
	EXPECT_FLOAT_EQ(body.Position().Y, position.Y);

	EXPECT_FLOAT_EQ(body.Velocity().X, velocity.X);
	EXPECT_FLOAT_EQ(body.Velocity().Y, velocity.Y);

	EXPECT_FLOAT_EQ(body.Force().X, 0.f);
	EXPECT_FLOAT_EQ(body.Force().Y, 0.f);
}

TEST(World, TriggerCircle)
{
    HeapAllocator allocator;
    World world;

	auto bodyRef2 = world.CreateBody();
	auto colliderRef2 = world.CreateCollider(bodyRef2);
	auto& collider2 = world.GetCollider(colliderRef2);
	auto interaction = Interaction::None;
	auto interactionCount = 0;
	auto* contactListener = new TestContactListener(interaction, interactionCount);

	world.SetContactListener(contactListener);
	collider2.SetCircle(CircleF({0.1f, 0.1f}, 1.f));
	collider2.SetIsTrigger(true);

	EXPECT_EQ(colliderRef2.Index, 0);
	EXPECT_EQ(colliderRef2.Generation, 0);
	EXPECT_EQ(collider2.GetBodyRef(), bodyRef2);
	EXPECT_TRUE(collider2.IsEnabled());

	auto bodyRef3 = world.CreateBody();
	auto colliderRef3 = world.CreateCollider(bodyRef3);
	auto& collider3 = world.GetCollider(colliderRef3);

	collider3.SetCircle(CircleF({0.f, 0.f}, 1.f));

	EXPECT_EQ(colliderRef3.Index, 1);
	EXPECT_EQ(colliderRef3.Generation, 0);

	// Check that the contact listener is called when the collider is a trigger
	world.Update(1.f / 60.f);

	EXPECT_EQ(interaction, Interaction::Enter);
	EXPECT_EQ(interactionCount, 1);

	world.Update(1.f / 60.f);

	EXPECT_EQ(interaction, Interaction::Stay);
	EXPECT_EQ(interactionCount, 2);

	world.GetBody(bodyRef2).SetPosition({ 10.f, 10.f });
	world.Update(1.f / 60.f);

	EXPECT_EQ(interaction, Interaction::Exit);
	EXPECT_EQ(interactionCount, 3);

	world.DestroyBody(bodyRef2);
	world.DestroyBody(bodyRef3);
}

TEST(World, TriggerRectangle)
{
    HeapAllocator allocator;
    World world;

	auto bodyRef2 = world.CreateBody();
	auto colliderRef2 = world.CreateCollider(bodyRef2);
	auto& collider2 = world.GetCollider(colliderRef2);
	auto interaction = Interaction::None;
	auto interactionCount = 0;
	auto* contactListener = new TestContactListener(interaction, interactionCount);

	world.SetContactListener(contactListener);
	collider2.SetRectangle(RectangleF({0.1f, 0.1f}, {1.f, 1.f}));
	collider2.SetIsTrigger(true);

	EXPECT_EQ(colliderRef2.Index, 0);
	EXPECT_EQ(colliderRef2.Generation, 0);
	EXPECT_EQ(collider2.GetBodyRef(), bodyRef2);
	EXPECT_TRUE(collider2.IsEnabled());

	auto bodyRef3 = world.CreateBody();
	auto colliderRef3 = world.CreateCollider(bodyRef3);
	auto& collider3 = world.GetCollider(colliderRef3);

	collider3.SetRectangle(RectangleF({0.f, 0.f}, {1.f, 1.f}));

	EXPECT_EQ(colliderRef3.Index, 1);
	EXPECT_EQ(colliderRef3.Generation, 0);

	// Check that the contact listener is called when the collider is a trigger
	world.Update(1.f / 60.f);

	EXPECT_EQ(interaction, Interaction::Enter);
	EXPECT_EQ(interactionCount, 1);

	world.Update(1.f / 60.f);

	EXPECT_EQ(interaction, Interaction::Stay);
	EXPECT_EQ(interactionCount, 2);

	world.GetBody(bodyRef2).SetPosition({ 10.f, 10.f });
	world.Update(1.f / 60.f);

	EXPECT_EQ(interaction, Interaction::Exit);
	EXPECT_EQ(interactionCount, 3);

	world.DestroyBody(bodyRef2);
	world.DestroyBody(bodyRef3);
}

TEST(World, TriggerPolygon)
{
    HeapAllocator allocator;
    World world;

	auto bodyRef2 = world.CreateBody();
	auto colliderRef2 = world.CreateCollider(bodyRef2);
	auto& collider2 = world.GetCollider(colliderRef2);
	auto interaction = Interaction::None;
	auto interactionCount = 0;
	auto* contactListener = new TestContactListener(interaction, interactionCount);

	world.SetContactListener(contactListener);
	collider2.SetPolygon(PolygonF({ {0.1f, 0.1f}, {0.8f, 0.1f}, {0.8f, 0.8f} }));
	collider2.SetIsTrigger(true);

	EXPECT_EQ(colliderRef2.Index, 0);
	EXPECT_EQ(colliderRef2.Generation, 0);
	EXPECT_EQ(collider2.GetBodyRef(), bodyRef2);
	EXPECT_TRUE(collider2.IsEnabled());

	auto bodyRef3 = world.CreateBody();
	auto colliderRef3 = world.CreateCollider(bodyRef3);
	auto& collider3 = world.GetCollider(colliderRef3);

	collider3.SetPolygon(PolygonF({ {0.f, 0.f}, {8.f, 0.f}, {8.f, 8.f} }));

	EXPECT_EQ(colliderRef3.Index, 1);
	EXPECT_EQ(colliderRef3.Generation, 0);

	// Check that the contact listener is called when the collider is a trigger
	world.Update(1.f / 60.f);

	EXPECT_EQ(interaction, Interaction::Enter);
	EXPECT_EQ(interactionCount, 1);

	world.Update(1.f / 60.f);

	EXPECT_EQ(interaction, Interaction::Stay);
	EXPECT_EQ(interactionCount, 2);

	world.GetBody(bodyRef2).SetPosition({ 100.f, 100.f });
	world.Update(1.f / 60.f);

	EXPECT_EQ(interaction, Interaction::Exit);
	EXPECT_EQ(interactionCount, 3);

	world.DestroyBody(bodyRef2);
	world.DestroyBody(bodyRef3);
}

TEST(World, CollisionCircle)
{
	HeapAllocator allocator;
	World world;

	auto bodyRef2 = world.CreateBody();
	auto colliderRef2 = world.CreateCollider(bodyRef2);
	auto& collider2 = world.GetCollider(colliderRef2);
	auto interaction = Interaction::None;
	auto interactionCount = 0;
	auto* contactListener = new TestContactListener(interaction, interactionCount);

	world.SetContactListener(contactListener);
	collider2.SetCircle(CircleF({0.1f, 0.1f}, 1.f));
	collider2.SetIsTrigger(false);

	EXPECT_EQ(colliderRef2.Index, 0);
	EXPECT_EQ(colliderRef2.Generation, 0);
	EXPECT_EQ(collider2.GetBodyRef(), bodyRef2);
	EXPECT_TRUE(collider2.IsEnabled());

	auto bodyRef3 = world.CreateBody();
	auto colliderRef3 = world.CreateCollider(bodyRef3);
	auto& collider3 = world.GetCollider(colliderRef3);

	collider3.SetCircle(CircleF({0.f, 0.f}, 1.f));

	EXPECT_EQ(colliderRef3.Index, 1);
	EXPECT_EQ(colliderRef3.Generation, 0);

	world.Update(1.f / 60.f);

	EXPECT_EQ(interaction, Interaction::Enter);
	EXPECT_EQ(interactionCount, 1);

	world.Update(1.f / 60.f);

	EXPECT_EQ(interaction, Interaction::Stay);
	EXPECT_EQ(interactionCount, 2);

	world.GetBody(bodyRef2).SetPosition({ 10.f, 10.f });
	world.Update(1.f / 60.f);

	EXPECT_EQ(interaction, Interaction::Exit);
	EXPECT_EQ(interactionCount, 3);

	world.DestroyBody(bodyRef2);
	world.DestroyBody(bodyRef3);
}

TEST(World, CollisionRectangle)
{
	HeapAllocator allocator;
	World world;

	auto bodyRef2 = world.CreateBody();
	auto colliderRef2 = world.CreateCollider(bodyRef2);
	auto& collider2 = world.GetCollider(colliderRef2);
	auto interaction = Interaction::None;
	auto interactionCount = 0;
	auto* contactListener = new TestContactListener(interaction, interactionCount);

	world.SetContactListener(contactListener);
	collider2.SetRectangle(RectangleF({0.1f, 0.1f}, {1.f, 1.f}));
	collider2.SetIsTrigger(false);

	EXPECT_EQ(colliderRef2.Index, 0);
	EXPECT_EQ(colliderRef2.Generation, 0);
	EXPECT_EQ(collider2.GetBodyRef(), bodyRef2);
	EXPECT_TRUE(collider2.IsEnabled());

	auto bodyRef3 = world.CreateBody();
	auto colliderRef3 = world.CreateCollider(bodyRef3);
	auto& collider3 = world.GetCollider(colliderRef3);

	collider3.SetRectangle(RectangleF({0.f, 0.f}, {1.f, 1.f}));

	EXPECT_EQ(colliderRef3.Index, 1);
	EXPECT_EQ(colliderRef3.Generation, 0);

	world.Update(1.f / 60.f);

	EXPECT_EQ(interaction, Interaction::Enter);
	EXPECT_EQ(interactionCount, 1);

	world.Update(1.f / 60.f);

	EXPECT_EQ(interaction, Interaction::Stay);
	EXPECT_EQ(interactionCount, 2);

	world.GetBody(bodyRef2).SetPosition({ 10.f, 10.f });
	world.Update(1.f / 60.f);

	EXPECT_EQ(interaction, Interaction::Exit);
	EXPECT_EQ(interactionCount, 3);

	world.DestroyBody(bodyRef2);
	world.DestroyBody(bodyRef3);
}

TEST(World, CollisionCircleAndRectangle)
{
	HeapAllocator allocator;
	World world;

	auto bodyRef2 = world.CreateBody();
	auto colliderRef2 = world.CreateCollider(bodyRef2);
	auto& collider2 = world.GetCollider(colliderRef2);
	auto interaction = Interaction::None;
	auto interactionCount = 0;
	auto* contactListener = new TestContactListener(interaction, interactionCount);

	world.SetContactListener(contactListener);
	collider2.SetCircle(CircleF({0.1f, 0.1f}, 1.f));
	collider2.SetIsTrigger(false);

	EXPECT_EQ(colliderRef2.Index, 0);
	EXPECT_EQ(colliderRef2.Generation, 0);
	EXPECT_EQ(collider2.GetBodyRef(), bodyRef2);
	EXPECT_TRUE(collider2.IsEnabled());

	auto bodyRef3 = world.CreateBody();
	auto colliderRef3 = world.CreateCollider(bodyRef3);
	auto& collider3 = world.GetCollider(colliderRef3);

	collider3.SetRectangle(RectangleF({0.f, 0.f}, {1.f, 1.f}));

	EXPECT_EQ(colliderRef3.Index, 1);
	EXPECT_EQ(colliderRef3.Generation, 0);

	world.Update(1.f / 60.f);

	EXPECT_EQ(interaction, Interaction::Enter);
	EXPECT_EQ(interactionCount, 1);

	world.Update(1.f / 60.f);

	EXPECT_EQ(interaction, Interaction::Stay);
	EXPECT_EQ(interactionCount, 2);

	world.GetBody(bodyRef2).SetPosition({ 10.f, 10.f });
	world.Update(1.f / 60.f);

	EXPECT_EQ(interaction, Interaction::Exit);
	EXPECT_EQ(interactionCount, 3);

	world.DestroyBody(bodyRef2);
	world.DestroyBody(bodyRef3);
}
#include "Collider.h"
#include "ColliderPair.h"

#include <gtest/gtest.h>

#include <array>

using namespace Physics;
using namespace Math;

struct TestColliderPairFixture : public ::testing::TestWithParam<std::array<ColliderRef, 4>> {};

INSTANTIATE_TEST_SUITE_P(ColliderPair, TestColliderPairFixture, testing::Values(
	std::array<ColliderRef, 4>{ ColliderRef(0, 0), ColliderRef(0, 1), ColliderRef(1, 0), ColliderRef(1, 1) },
	std::array<ColliderRef, 4>{ ColliderRef(0, 0), ColliderRef(1, 0), ColliderRef(0, 1), ColliderRef(1, 1) },
	std::array<ColliderRef, 4>{ ColliderRef(0, 0), ColliderRef(1, 1), ColliderRef(0, 1), ColliderRef(1, 0) },
	std::array<ColliderRef, 4>{ ColliderRef(0, 1), ColliderRef(0, 0), ColliderRef(1, 0), ColliderRef(1, 1) },
	std::array<ColliderRef, 4>{ ColliderRef(0, 1), ColliderRef(1, 0), ColliderRef(0, 0), ColliderRef(1, 1) },
	std::array<ColliderRef, 4>{ ColliderRef(0, 1), ColliderRef(1, 1), ColliderRef(0, 0), ColliderRef(1, 0) },
	std::array<ColliderRef, 4>{ ColliderRef(1, 0), ColliderRef(0, 0), ColliderRef(0, 1), ColliderRef(1, 1) },
	std::array<ColliderRef, 4>{ ColliderRef(1, 0), ColliderRef(0, 1), ColliderRef(0, 0), ColliderRef(1, 1) },
	std::array<ColliderRef, 4>{ ColliderRef(1, 0), ColliderRef(0, 1), ColliderRef(1, 1), ColliderRef(0, 0) },
	std::array<ColliderRef, 4>{ ColliderRef(1, 1), ColliderRef(0, 0), ColliderRef(0, 1), ColliderRef(1, 0) },
	std::array<ColliderRef, 4>{ ColliderRef(1, 1), ColliderRef(0, 1), ColliderRef(0, 0), ColliderRef(1, 0) },
	std::array<ColliderRef, 4>{ ColliderRef(1, 1), ColliderRef(0, 1), ColliderRef(1, 0), ColliderRef(0, 0) }
));

TEST(Collider, DefaultConstructor)
{
	Collider collider;

	EXPECT_EQ(collider.GetColliderRef(), ColliderRef());
	EXPECT_EQ(collider.GetBodyRef(), BodyRef());
	EXPECT_EQ(collider.GetRestitution(), 0.f);
	EXPECT_FALSE(collider.IsTrigger());
	EXPECT_TRUE(collider.IsFree());
	EXPECT_FALSE(collider.IsEnabled());
}

TEST(Collider, SetColliderRef)
{
	Collider collider;

	collider.SetColliderRef({ 1, 2 });

	EXPECT_EQ(collider.GetColliderRef(), ColliderRef(1, 2));
}

TEST(Collider, SetBodyRef)
{
	Collider collider;

	collider.SetBodyRef({ 1, 2 });

	EXPECT_EQ(collider.GetBodyRef(), BodyRef(1, 2));
}

TEST(Collider, SetBounciness)
{
	Collider collider;

	collider.SetBounciness(0.5f);

	EXPECT_EQ(collider.GetRestitution(), 0.5f);
}

TEST(Collider, SetIsTrigger)
{
	Collider collider;

	collider.SetIsTrigger(true);

	EXPECT_TRUE(collider.IsTrigger());
}

TEST(Collider, SetIsEnabled)
{
	Collider collider;

	collider.Enable();
	collider.SetCircle(CircleF({ 1.f, 2.f }, 3.f));

	EXPECT_TRUE(collider.IsEnabled());

	collider.Disable();

	EXPECT_FALSE(collider.IsEnabled());
}

TEST(Collider, SetCircle)
{
	Collider collider;
	CircleF circle({ 1.f, 2.f }, 3.f);

	collider.SetCircle(circle);

	EXPECT_EQ(collider.GetShapeType(), ShapeType::Circle);
	EXPECT_EQ(collider.GetCircle().Center(), circle.Center());
	EXPECT_EQ(collider.GetCircle().Radius(), circle.Radius());
}

TEST(Collider, SetRectangle)
{
	Collider collider;
	RectangleF rectangle({ 1.f, 2.f }, { 3.f, 4.f });

	collider.SetRectangle(rectangle);

	EXPECT_EQ(collider.GetShapeType(), ShapeType::Rectangle);
	EXPECT_EQ(collider.GetRectangle().MinBound(), rectangle.MinBound());
	EXPECT_EQ(collider.GetRectangle().MaxBound(), rectangle.MaxBound());
}

TEST(Collider, SetPolygon)
{
	Collider collider;
	PolygonF polygon({ { 1.f, 2.f }, { 3.f, 4.f }, { 5.f, 6.f } });

	collider.SetPolygon(polygon);

	EXPECT_EQ(collider.GetShapeType(), ShapeType::Polygon);
	EXPECT_EQ(collider.GetPolygon().Vertices(), polygon.Vertices());
}

TEST(ColliderPair, DefaultConstructor)
{
	ColliderPair colliderPair{};

	EXPECT_EQ(colliderPair.A, ColliderRef());
	EXPECT_EQ(colliderPair.B, ColliderRef());
}

TEST_P(TestColliderPairFixture, Constructor)
{
	const auto& params = GetParam();
	const auto& colliderRef1 = params[0];
	const auto& colliderRef2 = params[1];

	ColliderPair colliderPair{ colliderRef1, colliderRef2 };

	EXPECT_EQ(colliderPair.A, colliderRef1);
	EXPECT_EQ(colliderPair.B, colliderRef2);
}

TEST_P(TestColliderPairFixture, EqualityOperator)
{
	const auto& params = GetParam();
	const auto& colliderRef1 = params[0];
	const auto& colliderRef2 = params[1];
	const auto& colliderRef3 = params[2];
	const auto& colliderRef4 = params[3];

	ColliderPair colliderPair1{ colliderRef1, colliderRef2 };
	ColliderPair colliderPair2{ colliderRef3, colliderRef4 };

	EXPECT_EQ(colliderPair1 == colliderPair2,
	          colliderPair1.A == colliderPair2.A && colliderPair1.B == colliderPair2.B
			  || colliderPair1.A == colliderPair2.B && colliderPair1.B == colliderPair2.A);
}
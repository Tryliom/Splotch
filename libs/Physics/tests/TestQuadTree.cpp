#include "QuadTree.h"

#include <gtest/gtest.h>

struct TestQuadTreeFixture : public ::testing::TestWithParam<Math::RectangleF> {};

INSTANTIATE_TEST_SUITE_P(QuadTree, TestQuadTreeFixture, testing::Values(
	Math::RectangleF(Math::Vec2F(0, 0), Math::Vec2F(1, 1)),
	Math::RectangleF(Math::Vec2F(0.5f, 1.f), Math::Vec2F(5.f, 15.f)),
	Math::RectangleF(Math::Vec2F(0.f, 0.f), Math::Vec2F(300.f, 300.f)),
	Math::RectangleF(Math::Vec2F(-500.f, -700.f), Math::Vec2F(1'000.f, 1'200.f))
));

TEST_P(TestQuadTreeFixture, DefaultConstructor)
{
	Physics::QuadTree quadTree(GetParam());
	auto boundaries = quadTree.GetBoundaries();

	EXPECT_EQ(boundaries.size(), 0);
}

TEST_P(TestQuadTreeFixture, InsertTopLeft)
{
	auto rect = GetParam();
	Physics::QuadTree quadTree(rect);
	Math::Vec2F collidersSize = rect.Size() / 100.f;
	auto topLeftRect = Math::RectangleF(rect.MinBound() - collidersSize / 2.f, rect.MinBound() + collidersSize / 2.f);

	std::vector<Physics::SimplifiedCollider> colliders;

	for (std::size_t i = 0; i < Physics::QuadTree::MaxCapacity(); i++)
	{
		colliders.push_back({{i, 0}, topLeftRect});
		quadTree.Insert(colliders.back());
	}

	EXPECT_EQ(quadTree.GetBoundaries().size(), 1);
	EXPECT_EQ(quadTree.GetAllCollidersCount(), colliders.size());

	colliders.push_back({{colliders.size(), 0}, topLeftRect});
	quadTree.Insert(colliders.back());

	// Expect that all colliders will move to the next boundary until the last depth and cannot be divided anymore
	EXPECT_EQ(quadTree.GetBoundaries().size(), 1);
	EXPECT_EQ(quadTree.GetAllCollidersCount(), colliders.size());
}

TEST_P(TestQuadTreeFixture, InsertMid)
{
	auto rect = GetParam();
	Physics::QuadTree quadTree(rect);
	Math::Vec2F collidersSize = rect.Size() / 100.f;
	Math::Vec2F center = rect.Center();
	Math::RectangleF middleRect(center - collidersSize / 2.f, center + collidersSize / 2.f);
	Math::RectangleF topLeftRect(rect.MinBound() - collidersSize / 2.f, rect.MinBound() + collidersSize / 2.f);

	std::vector<Physics::SimplifiedCollider> colliders;

	for (std::size_t i = 0; i < Physics::QuadTree::MaxCapacity(); i++)
	{
		colliders.push_back({{i, 0}, middleRect});
		quadTree.Insert(colliders.back());
	}

	EXPECT_EQ(quadTree.GetBoundaries().size(), 1);
	EXPECT_EQ(quadTree.GetAllCollidersCount(), colliders.size());

	colliders.push_back({{colliders.size(), 0}, middleRect});
	quadTree.Insert(colliders.back());

	// Expect that all colliders are on the four children of the root node, so they are kept in the same boundary
	EXPECT_EQ(quadTree.GetBoundaries().size(), 1);
	EXPECT_EQ(quadTree.GetAllCollidersCount(), colliders.size());

	colliders.push_back({{colliders.size(), 0}, topLeftRect});
	quadTree.Insert(colliders.back());

 	EXPECT_EQ(quadTree.GetBoundaries().size(), 1 + 1); // Main boundary + topleft boundary
	EXPECT_EQ(quadTree.GetAllCollidersCount(), colliders.size());
}

TEST_P(TestQuadTreeFixture, GetColliders)
{
	auto rect = GetParam();
	Physics::QuadTree quadTree(rect);
	Math::Vec2F collidersSize = rect.Size() / 100.f;
	Math::Vec2F center = rect.Center();
	Math::RectangleF middleRect(center - collidersSize / 2.f, center + collidersSize / 2.f);

	std::vector<Physics::SimplifiedCollider> colliders;

	for (std::size_t i = 0; i < Physics::QuadTree::MaxCapacity(); i++)
	{
		colliders.push_back({{i, 0}, middleRect});
		quadTree.Insert(colliders.back());
	}

	EXPECT_EQ(quadTree.GetBoundaries().size(), 1);
	EXPECT_EQ(quadTree.GetAllCollidersCount(), colliders.size());

	colliders.push_back({{colliders.size(), 0}, middleRect});
	quadTree.Insert(colliders.back());

	auto collidersToCheck = quadTree.GetAllPossiblePairs();

	EXPECT_EQ(collidersToCheck.size(), colliders.size() * 4);
}

TEST_P(TestQuadTreeFixture, ClearColliders)
{
	auto rect = GetParam();
	Physics::QuadTree quadTree(rect);
	Math::Vec2F collidersSize = rect.Size() / 100.f;
	Math::Vec2F center = rect.Center();
	Math::RectangleF middleRect(center - collidersSize / 2.f, center + collidersSize / 2.f);

	std::vector<Physics::SimplifiedCollider> colliders;

	for (std::size_t i = 0; i < Physics::QuadTree::MaxCapacity(); i++)
	{
		colliders.push_back({{i, 0}, middleRect});
		quadTree.Insert(colliders.back());
	}

	EXPECT_EQ(quadTree.GetBoundaries().size(), 1);
	EXPECT_EQ(quadTree.GetAllCollidersCount(), colliders.size());

	quadTree.ClearColliders();

	EXPECT_EQ(quadTree.GetBoundaries().size(), 0);
	EXPECT_EQ(quadTree.GetAllCollidersCount(), 0);
}

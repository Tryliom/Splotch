#include "World.h"

#include "Exception.h"
#include "ContactResolver.h"

#ifdef TRACY_ENABLE
#include <tracy/Tracy.hpp>
#include <fmt/format.h>
#endif

namespace Physics
{
	World::World(std::size_t defaultBodySize) noexcept :
		_lastColliderPairs{StandardAllocator<ColliderPair> {_heapAllocator} },
		_bodies { StandardAllocator<Body> {_heapAllocator} },
		_colliders { StandardAllocator<Collider> {_heapAllocator} },
		_colliderGenerations { StandardAllocator<std::size_t> {_heapAllocator} },
		_bodyGenerations { StandardAllocator<std::size_t> {_heapAllocator} }
	{
		if (defaultBodySize == 0)
		{
			defaultBodySize = 1;
		}

		_bodies.resize(defaultBodySize);
		_bodyGenerations.resize(defaultBodySize, 0);
		_colliders.resize(defaultBodySize);
		_colliderGenerations.resize(defaultBodySize, 0);
	}

	void World::updateColliders() noexcept
	{
#ifdef TRACY_ENABLE
		ZoneNamedN(updateColliders, "World::updateColliders", true);
#endif
		// Calculate minimum and maximum bounds of all colliders
		float minX = std::numeric_limits<float>::max();
		float minY = std::numeric_limits<float>::max();
		float maxX = std::numeric_limits<float>::min();
		float maxY = std::numeric_limits<float>::min();

		for (auto& collider : _colliders)
		{
			if (!collider.IsEnabled() || collider.IsFree()) continue;

			const auto& bounds = collider.GetBounds();

			const auto& min = bounds.MinBound();
			const auto& max = bounds.MaxBound();

			if (min.X < minX) minX = min.X;
			if (min.Y < minY) minY = min.Y;
			if (max.X > maxX) maxX = max.X;
			if (max.Y > maxY) maxY = max.Y;
		}

		// Clear all colliders from the quadtree
		_quadTree.ClearColliders();

		// Update the boundary of the quadtree
		_quadTree.UpdateBoundary(Math::RectangleF({ minX, minY }, { maxX, maxY }));

		// Insert all colliders into the quadtree
		insertColliders();

		// Check for collisions and triggers
		processColliders();
	}

	void World::insertColliders() noexcept
	{
#ifdef TRACY_ENABLE
		ZoneNamedN(insertColliders, "World::insertColliders", true);
#endif

		for (auto& collider : _colliders)
		{
			if (!collider.IsEnabled() || collider.IsFree()) continue;

			_quadTree.Insert({collider.GetColliderRef(), collider.GetBounds()});
		}
	}

    MyVector<ColliderPair> World::getColliderPairs() noexcept
    {
#ifdef TRACY_ENABLE
        ZoneScopedN("World::getColliderPairs");
#endif

        const auto& allPossibleColliderPairs = _quadTree.GetAllPossiblePairs();
        MyVector<ColliderPair> newColliderPairs { StandardAllocator<ColliderPair> {_heapAllocator} };

        newColliderPairs.reserve(allPossibleColliderPairs.size());

        for (const auto& colliderPair : allPossibleColliderPairs)
        {
            const Collider& colliderA = GetCollider(colliderPair.A);
            const Collider& colliderB = GetCollider(colliderPair.B);

            if (colliderA.GetBodyRef() == colliderB.GetBodyRef()) continue;

            if (colliderA.GetShapeType() == Math::ShapeType::Rectangle && colliderB.GetShapeType() == Math::ShapeType::Rectangle ||
                overlap(colliderA, colliderB))
            {
                newColliderPairs.push_back(colliderPair);
            }
        }

#ifdef TRACY_ENABLE

        const auto& info = fmt::format(
                "{} - {} = {} verified collider pairs",
                allPossibleColliderPairs.size(), allPossibleColliderPairs.size() - newColliderPairs.size(), newColliderPairs.size());
        ZoneText(info.c_str(), info.size());
#endif

        return newColliderPairs;
    }

	void World::processColliders() noexcept
	{
#ifdef TRACY_ENABLE
        ZoneScopedN("World::processColliders");
#endif
		MyVector<ColliderPair> newColliderPairs = getColliderPairs();

#ifdef TRACY_ENABLE
        ZoneNamedN(onCollisions, "Check triggers and collisions", true);
#endif

		for (const auto& collider : newColliderPairs)
		{
			const Collider& colliderA = GetCollider(collider.A);
			const Collider& colliderB = GetCollider(collider.B);

			if (std::find(_lastColliderPairs.begin(), _lastColliderPairs.end(), collider) == _lastColliderPairs.end())
			{
				if (_contactListener == nullptr) continue;

				// Enter
				if (colliderA.IsTrigger() || colliderB.IsTrigger())
				{
					_contactListener->OnTriggerEnter(collider.A, collider.B);
				}
				else
				{
					_contactListener->OnCollisionEnter(collider.A, collider.B);
				}
			}
			else
			{
				// Stay
				if (colliderA.IsTrigger() || colliderB.IsTrigger())
				{
					if (_contactListener == nullptr) continue;

					_contactListener->OnTriggerStay(collider.A, collider.B);
				}
				else
				{
					if (_contactListener != nullptr)
					{
						_contactListener->OnCollisionStay(collider.A, collider.B);
					}

					const auto& bodyA = GetBody(colliderA.GetBodyRef());
					const auto& bodyB = GetBody(colliderB.GetBodyRef());

					if (bodyA.GetBodyType() == BodyType::Dynamic || bodyB.GetBodyType() == BodyType::Dynamic)
					{
						onCollision(collider.A, collider.B);
					}
				}
			}
		}

		if (_contactListener != nullptr)
		{
			// Exit
			for (auto& colliderPair: _lastColliderPairs)
			{
				if (std::find(newColliderPairs.begin(), newColliderPairs.end(), colliderPair) != newColliderPairs.end()) continue;

				Collider& colliderA = GetCollider(colliderPair.A);
				Collider& colliderB = GetCollider(colliderPair.B);

				if (colliderA.IsTrigger() || colliderB.IsTrigger())
				{
					_contactListener->OnTriggerExit(colliderPair.A, colliderPair.B);
				}
				else
				{
					_contactListener->OnCollisionExit(colliderPair.A, colliderPair.B);
				}
			}
		}

		_lastColliderPairs = newColliderPairs;
	}

	void World::onCollision(Physics::ColliderRef colliderRef, Physics::ColliderRef otherColliderRef) noexcept
	{
#ifdef TRACY_ENABLE
		ZoneNamedN(onCollision, "World::onCollision", true);
#endif

		ContactResolver resolver(
			&GetBody(GetCollider(colliderRef).GetBodyRef()),
			&GetBody(GetCollider(otherColliderRef).GetBodyRef()),
			&GetCollider(colliderRef),
			&GetCollider(otherColliderRef)
		);

		resolver.ResolveContact();
	}

	bool World::overlap(const Collider& colliderA, const Collider& colliderB) noexcept
	{
#ifdef TRACY_ENABLE
		ZoneNamedN(overlap, "World::overlap", true);
#endif

		if (colliderA.GetBodyRef() == colliderB.GetBodyRef()) return false;

        switch (colliderA.GetShapeType())
        {
            case Math::ShapeType::Circle:
            {
                auto circleA = Math::CircleF(colliderA.GetOffset() + colliderA.GetPosition(), colliderA.GetCircle().Radius());

                switch (colliderB.GetShapeType())
                {
                    case Math::ShapeType::Circle:
                    {
                        auto circleB = Math::CircleF(colliderB.GetOffset() + colliderB.GetPosition(), colliderB.GetCircle().Radius());

                        return Math::Intersect(circleA, circleB);
                    }
                    case Math::ShapeType::Rectangle:
                    {
                        auto rectB = colliderB.GetRectangle() + colliderB.GetPosition() + colliderB.GetOffset();

                        return Math::Intersect(circleA, rectB);
                    }
                    case Math::ShapeType::Polygon:
                    {
                        auto polyB = colliderB.GetPolygon() + colliderB.GetPosition() + colliderB.GetOffset();

                        return Math::Intersect(circleA, polyB);
                    }
	                case Math::ShapeType::None:break;
                }
            }
            break;

            case Math::ShapeType::Rectangle:
            {
                auto rectA = colliderA.GetRectangle() + colliderA.GetPosition() + colliderA.GetOffset();

                switch (colliderB.GetShapeType())
                {
                    case Math::ShapeType::Circle:
                    {
                        auto circleB = Math::CircleF(colliderB.GetOffset() + colliderB.GetPosition(), colliderB.GetCircle().Radius());

                        return Math::Intersect(rectA, circleB);
                    }
                    case Math::ShapeType::Rectangle:
                    {
                        auto rectB = colliderB.GetRectangle() + colliderB.GetPosition() + colliderB.GetOffset();

                        return Math::Intersect(rectA, rectB);
                    }
                    case Math::ShapeType::Polygon:
                    {
                        auto polyB = colliderB.GetPolygon() + colliderB.GetPosition() + colliderB.GetOffset();

                        return Math::Intersect(rectA, polyB);
                    }
	                case Math::ShapeType::None:break;
                }
            }
            break;

            case Math::ShapeType::Polygon:
            {
                auto polyA = colliderA.GetPolygon() + colliderA.GetPosition();

                switch (colliderB.GetShapeType())
                {
                    case Math::ShapeType::Circle:
                    {
                        auto circleB = Math::CircleF(colliderB.GetOffset() + colliderB.GetPosition(), colliderB.GetCircle().Radius());

                        return Math::Intersect(polyA, circleB);
                    }
                    case Math::ShapeType::Rectangle:
                    {
                        auto rectB = colliderB.GetRectangle() + colliderB.GetPosition() + colliderB.GetOffset();

                        return Math::Intersect(polyA, rectB);
                    }
                    case Math::ShapeType::Polygon:
                    {
                        auto polyB = colliderB.GetPolygon() + colliderB.GetPosition() + colliderB.GetOffset();

                        return Math::Intersect(polyA, polyB);
                    }
	                case Math::ShapeType::None:break;
                }
            }
            break;
            case Math::ShapeType::None: break;
        }

		return false;
	}

	void World::updateBodies(float deltaTime) noexcept
	{
#ifdef TRACY_ENABLE
		ZoneNamedN(updateBodies, "World::updateBodies", true);
#endif
		for (auto& body : _bodies)
		{
			if (!body.IsEnabled()) continue;

			switch(body.GetBodyType())
			{
				case BodyType::Static: break;
				case BodyType::Dynamic:
				{
					if (body.UseGravity())
					{
						body.AddForce(_gravity);
					}

					body.AddVelocity(body.Force() * body.InverseMass() * deltaTime);
					body.AddPosition(body.Velocity() * deltaTime);
					body.SetForce(Math::Vec2F(0, 0));
				}
				break;
				case BodyType::Kinematic:
				{
					body.AddPosition(body.Velocity() * deltaTime);
				}
				break;
			}
		}

		if (_colliders.empty()) return;

		for (auto& collider : _colliders)
		{
			if (!collider.IsEnabled()) continue;

			const auto& body = GetBody(collider.GetBodyRef());

			collider.SetPosition(body.Position() + collider.GetOffset());
		}
	}

	void World::Update(float deltaTime) noexcept
	{
#ifdef TRACY_ENABLE
		ZoneNamedN(update, "World::Update", true);
#endif
		updateBodies(deltaTime);
        updateColliders();
	}

	BodyRef World::CreateBody() noexcept
	{
		for (size_t i = 0; i < _bodies.size(); i++)
		{
			if (_bodies[i].IsEnabled()) continue;

			_bodies[i].Enable();

			return {i, _bodyGenerations[i] };
		}

		// No free bodies found, create a new one, and increase the size of the vector
        const std::size_t oldSize = _bodies.size();
		_bodies.resize(_bodies.size() * 2);
        _bodyGenerations.resize(_bodyGenerations.size() * 2);

		_bodies[oldSize].Enable();

		return {oldSize, _bodyGenerations[oldSize] };
	}

	void World::DestroyBody(BodyRef bodyRef)
	{
        if (_bodyGenerations[bodyRef.Index] != bodyRef.Generation)
        {
            throw InvalidBodyRefException();
        }

		for (std::size_t i = 0; i < _colliders.size(); i++)
		{
            auto& collider = _colliders[i];

			if (collider.IsFree()) continue;

			if (collider.GetBodyRef() == bodyRef)
			{
				DestroyCollider({ i, _colliderGenerations[i] });
            }
		}

		_bodies[bodyRef.Index].Disable();
		_bodyGenerations[bodyRef.Index]++;
	}

	Body& World::GetBody(BodyRef bodyRef)
	{
		if (_bodyGenerations[bodyRef.Index] != bodyRef.Generation)
		{
			throw InvalidBodyRefException();
		}

		return _bodies[bodyRef.Index];
	}

	ColliderRef World::CreateCollider(BodyRef bodyRef) noexcept
	{
		for (size_t i = 0; i < _colliders.size(); i++)
		{
			if (!_colliders[i].IsFree()) continue;

			const ColliderRef colliderRef = { i, _colliderGenerations[i] };

			_colliders[i].SetBodyRef(bodyRef);
			_colliders[i].Enable();
			_colliders[i].SetColliderRef(colliderRef);

			return colliderRef;
		}

		// No free colliders found, create a new one, and increase the size of the vector
		const std::size_t oldSize = _colliders.size();

		_colliders.resize(_colliders.size() * 2);
		_colliderGenerations.resize(_colliderGenerations.size() * 2);

		const ColliderRef colliderRef = { oldSize, _colliderGenerations[oldSize] };

		_colliders[oldSize].SetBodyRef(bodyRef);
		_colliders[oldSize].Enable();
		_colliders[oldSize].SetColliderRef(colliderRef);

		return colliderRef;
	}

	void World::DestroyCollider(ColliderRef colliderRef)
	{
		_colliders[colliderRef.Index].Free();
		_colliderGenerations[colliderRef.Index]++;
	}

	Collider& World::GetCollider(ColliderRef colliderRef)
	{
		if (_colliderGenerations[colliderRef.Index] != colliderRef.Generation)
		{
			throw InvalidColliderRefException();
		}

		return _colliders[colliderRef.Index];
	}

    void World::SetContactListener(ContactListener* contactListener) noexcept
    {
        _contactListener = contactListener;
    }

	std::vector<Math::RectangleF> World::GetQuadTreeBoundaries() const noexcept
	{
		return _quadTree.GetBoundaries();
	}

    void World::SetGravity(Math::Vec2F gravity) noexcept
    {
        _gravity = gravity;
    }
}
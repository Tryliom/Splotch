#pragma once

#include "Body.h"
#include "Collider.h"

namespace Physics
{
	/**
	 * @brief A contact resolver is used to resolve the collision between two bodies
	 */
	class ContactResolver
	{
	public:
		/**
		 * @brief Construct a new Contact Resolver object
		 * @param bodyA The first body
		 * @param bodyB The second body
		 * @param colliderA The first collider
		 * @param colliderB The second collider
		 */
		ContactResolver(Body* bodyA, Body* bodyB, Collider* colliderA, Collider* colliderB) noexcept;
		~ContactResolver() noexcept = default;

	private:
		Body* _bodyA { nullptr };
		Body* _bodyB { nullptr };
		Collider* _colliderA { nullptr };
		Collider* _colliderB { nullptr };

		Math::Vec2F _normal { Math::Vec2F::Zero() };
		float _penetration { 0.f };

	public:
		/**
		 * @brief Calculate the contact between the two colliders, applying the correct forces to the bodies and resolving the collision
		 */
		void ResolveContact() noexcept;

	private:
		/**
		 * @brief Setup the contact between the two colliders
		 */
		void setupContact() noexcept;
		/**
		 * @brief Resolve the collision between the two colliders
		 */
		void resolveCollision() noexcept;
		/**
		 * @brief Resolve the position of the two colliders
		 */
		void resolvePosition() noexcept;

		/**
		 * @brief Resolve the collision between two circles
		 */
		void resolveCircleToCircle() noexcept;
		/**
		 * @brief Resolve the collision between two rectangles
		 */
		void resolveRectangleToRectangle() noexcept;
		/**
		 * @brief Resolve the collision between a circle and a rectangle
		 */
		void resolveCircleToRectangle() noexcept;
	};
}
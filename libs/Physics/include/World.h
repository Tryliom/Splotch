#pragma once

#include "Body.h"
#include "Collider.h"
#include "ColliderPair.h"
#include "ContactListener.h"
#include "QuadTree.h"
#include "Allocator.h"

#include <vector>
#include <unordered_set>

namespace Physics
{
    /**
     * @brief The world class is the main class of the Physics engine. It contains all bodies and colliders.
     */
    class World
    {
    public:
		/**
		 * @brief Construct a new World object
		 * @param defaultBodySize The default size of the bodies vector
		 */
        explicit World(std::size_t defaultBodySize = 500) noexcept;
		~World() noexcept = default;

    private:
		QuadTree _quadTree {Math::RectangleF(Math::Vec2F::Zero(), Math::Vec2F::One())};
	    HeapAllocator _heapAllocator;

		MyVector<ColliderPair> _lastColliderPairs;
	    MyVector<Body> _bodies;
		MyVector<Collider> _colliders;
	    MyVector<std::size_t> _colliderGenerations;
	    MyVector<std::size_t> _bodyGenerations;

        ContactListener* _contactListener { nullptr };

        Math::Vec2F _gravity;

		/**
		 * @brief Check the collisions and triggers of the colliders
		 */
		void updateColliders() noexcept;
		/**
		 * @brief Check the collisions and triggers of the colliders in the quadtree
		 */
		void insertColliders() noexcept;
		/**
		 * @brief Check the collisions and triggers of the colliders in the quadtree
		 */
        MyVector<ColliderPair> getColliderPairs() noexcept;
		void processColliders() noexcept;
		/**
		 * @brief Calculate the collisions of the colliders
		 * @param colliderRef The collider to check the collisions for
		 * @param otherColliderRef The other collider to check the collisions for
		 */
		void onCollision(ColliderRef colliderRef, ColliderRef otherColliderRef) noexcept;
		/**
		 * @brief Check if the colliders overlap
		 * @param colliderA	 The first collider
		 * @param colliderB  The second collider
		 * @return True if the colliders overlap
		 */
		[[nodiscard]] static bool overlap(const Collider& colliderA, const Collider& colliderB) noexcept;

		/**
		 * @brief Update the bodies
		 * @param deltaTime The time since the last update
		 */
		void updateBodies(float deltaTime) noexcept;

    public:
		/**
		 * @brief Update the world
		 * @param deltaTime The time since the last update
		 */
        void Update(float deltaTime) noexcept;

		/**
		 * @brief Create a body. Sets the bodyRef of the body. Enables the body
		 * @return The bodyRef of the created body
		 */
        BodyRef CreateBody() noexcept;
		/**
		 * @brief Destroy a body and all its colliders
		 * @param bodyRef The body to destroy
		 */
        void DestroyBody(BodyRef bodyRef);
		/**
		 * @brief Get a body
		 * @param bodyRef The bodyRef of the body
		 * @return The body
		 */
        Body& GetBody(BodyRef bodyRef);

		/**
		 * @brief Create a collider for a body. Sets the bodyRef and colliderRef of the collider. Enables the collider.
		 * @param bodyRef The body to create the collider for.
		 * @return The colliderRef of the created collider.
		 */
		ColliderRef CreateCollider(BodyRef bodyRef) noexcept;
		/**
		 * @brief Destroy a collider
		 * @param colliderRef The collider to destroy
		 */
		void DestroyCollider(ColliderRef colliderRef);
		/**
		 * @brief Get a collider
		 * @param colliderRef The colliderRef of the collider
		 * @return The collider
		 */
		Collider& GetCollider(ColliderRef colliderRef);

		/**
		 * @brief Set the contact listener of the world for collision and trigger events, there is only one callback for both events
		 * @param contactListener The contact listener
		 */
        void SetContactListener(ContactListener* contactListener) noexcept;

	    /**
		 * @brief Get all the boundaries of the quadtree
		 * @return All the boundaries of the quadtree
		 */
	    [[nodiscard]] std::vector<Math::RectangleF> GetQuadTreeBoundaries() const noexcept;

		/**
		 * @brief Get the gravity of the world
		 * @param gravity The gravity of the world
		 */
        void SetGravity(Math::Vec2F gravity) noexcept;
    };
}
#pragma once

#include "Ref.h"

namespace Physics
{
    /**
     * @brief A contact listener is used to listen for collisions and triggers between colliders.
     */
	class ContactListener
	{
	public:
        /**
         * @brief Called when two colliders collide.
         * @param colliderRef The colliderRef of the collider that collided.
         * @param otherColliderRef The colliderRef of the collider that collided with.
         */
		virtual void OnTriggerEnter(ColliderRef colliderRef, ColliderRef otherColliderRef) noexcept = 0;
        /**
         * @brief Called when two colliders stop colliding.
         * @param colliderRef The colliderRef of the collider that stopped colliding.
         * @param otherColliderRef The colliderRef of the collider that stopped colliding with.
         */
		virtual void OnTriggerExit(ColliderRef colliderRef, ColliderRef otherColliderRef) noexcept = 0;
        /**
         * @brief Called when two colliders are colliding.
         * @param colliderRef The colliderRef of the collider that is colliding.
         * @param otherColliderRef The colliderRef of the collider that is colliding with.
         */
		virtual void OnTriggerStay(ColliderRef colliderRef, ColliderRef otherColliderRef) noexcept = 0;

		/**
		 * @brief Called when two colliders collide.
		 * @param colliderRef The colliderRef of the collider that collided.
		 * @param otherColliderRef The colliderRef of the collider that collided with.
		 */
		virtual void OnCollisionEnter(ColliderRef colliderRef, ColliderRef otherColliderRef) noexcept = 0;
		/**
		 * @brief Called when two colliders stop colliding.
		 * @param colliderRef The colliderRef of the collider that stopped colliding.
		 * @param otherColliderRef The colliderRef of the collider that stopped colliding with.
		 */
		virtual void OnCollisionExit(ColliderRef colliderRef, ColliderRef otherColliderRef) noexcept = 0;
		/**
		 * @brief Called when two colliders are colliding.
		 * @param colliderRef The colliderRef of the collider that is colliding.
		 * @param otherColliderRef The colliderRef of the collider that is colliding with.
		 */
		virtual void OnCollisionStay(ColliderRef colliderRef, ColliderRef otherColliderRef) noexcept = 0;
	};
}
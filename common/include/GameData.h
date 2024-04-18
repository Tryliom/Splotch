#pragma once

#include "Constants.h"
#include "PlayerInputs.h"

#include "Vec2.h"
#include "World.h"

#include <SFML/System/Time.hpp>

struct Forces
{
	Math::Vec2F Force;
	Math::Vec2F Velocity;
};

class GameData : public Physics::ContactListener
{
public:
	Physics::World World;
	Math::Vec2F PlayerPosition;
	HandSlot Hand = HandSlot::SLOT_3; // Not physical, just for the player to know where is the hand

	Physics::BodyRef PlayerBody;
	Physics::ColliderRef PlayerBottomCollider;
	Physics::ColliderRef PlayerTopCollider;
	Physics::ColliderRef PlayerPhysicsCollider;

	Physics::ColliderRef LastBrick;

	bool IsPlayerOnGround = true;
	bool IsPlayerDead = false;

 protected:
	ScreenSizeValue _width{};
	ScreenSizeValue _height{};

	// Temporary values used in update
	PlayerInput _playerInputs{};
	PlayerInput _previousPlayerInputs{};
	PlayerInput _handInputs{};
	PlayerInput _previousHandInputs{};

	// Constants
	static constexpr float PLAYER_SPEED = 200.f;
	static constexpr Math::Vec2F PLAYER_JUMP = GRAVITY * -20.f;

	void DecreaseHandSlot();
	void IncreaseHandSlot();

	[[nodiscard]] Forces GetNextPlayerForces();

	void SetupWorld();

	void UpdatePlayer();
	void UpdateHand();

	/**
	 * @brief Spawn a brick at hand position
	 */
	void SpawnBrick();

 public:
	void StartGame(ScreenSizeValue width, ScreenSizeValue height);

	/**
	 * @brief Register the player inputs, need to be called before Update
	 * @param playerInput
	 * @param previousPlayerInput
	 * @param handInput
	 * @param previousHandInput
	 */
	void RegisterPlayersInputs(PlayerInput playerInput, PlayerInput previousPlayerInput, PlayerInput handInput, PlayerInput previousHandInput);
	void Update(sf::Time elapsed);

	[[nodiscard]] Math::Vec2F GetHandPosition() const;

	/**
	 * @brief Compare two GameData, players drawable are not compared
	 * @param other
	 * @return
	 */
	bool operator==(const GameData& other) const;

	[[nodiscard]] int GenerateChecksum() const;

	void OnTriggerEnter(Physics::ColliderRef colliderRef, Physics::ColliderRef otherColliderRef) noexcept override;
	void OnTriggerExit(Physics::ColliderRef colliderRef, Physics::ColliderRef otherColliderRef) noexcept override;
	void OnTriggerStay(Physics::ColliderRef colliderRef, Physics::ColliderRef otherColliderRef) noexcept override;

	void OnCollisionEnter(Physics::ColliderRef colliderRef, Physics::ColliderRef otherColliderRef) noexcept override {}
	void OnCollisionExit(Physics::ColliderRef colliderRef, Physics::ColliderRef otherColliderRef) noexcept override {}
	void OnCollisionStay(Physics::ColliderRef colliderRef, Physics::ColliderRef otherColliderRef) noexcept override {}
};
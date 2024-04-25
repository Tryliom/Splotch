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

struct Brick
{
	Physics::BodyRef Body;
	Physics::ColliderRef Collider;
	bool IsAlive = false;
};

class GameData : public Physics::ContactListener
{
public:
	Physics::World World;
	Math::Vec2F PlayerPosition;
	GhostSlot Ghost = GhostSlot::SLOT_3; // Not physical, just for the player to know where is the ghost

	Physics::BodyRef PlayerBody;
	Physics::ColliderRef PlayerBottomCollider;
	Physics::ColliderRef PlayerTopCollider;
	Physics::ColliderRef PlayerPhysicsCollider;

	Physics::ColliderRef PlatformCollider;

	std::array<std::array<Brick, MAX_BRICKS_PER_COLUMN>, HAND_SLOT_COUNT> BricksPerSlot;

	float BrickCooldown = COOLDOWN_SPAWN_BRICK;

	bool IsPlayerOnGround = true;
	bool IsPlayerDead = false;

 protected:
	ScreenSizeValue _width{};
	ScreenSizeValue _height{};

	// Temporary values used in update
	PlayerInput _playerInputs{};
	PlayerInput _previousPlayerInputs{};
	PlayerInput _ghostInputs{};
	PlayerInput _previousGhostInputs{};

	// Constants
	static constexpr float COOLDOWN_SPAWN_BRICK = 0.3f;
	static constexpr float PLAYER_SPEED = 200.f;
	static constexpr Math::Vec2F PLAYER_JUMP = GRAVITY * -20.f;

	void DecreaseGhostSlot();
	void IncreaseGhostSlot();

	[[nodiscard]] Forces GetNextPlayerForces();

	void SetupWorld();

	void UpdatePlayer();
	void UpdateGhost();

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
	 * @param ghostInput
	 * @param previousGhostInput
	 */
	void AddPlayersInputs(PlayerInput playerInput, PlayerInput previousPlayerInput, PlayerInput ghostInput, PlayerInput previousGhostInput);
	void FixedUpdate();

	[[nodiscard]] Math::Vec2F GetGhostPosition() const;

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

	void OnCollisionEnter(Physics::ColliderRef colliderRef, Physics::ColliderRef otherColliderRef) noexcept override;
	void OnCollisionExit(Physics::ColliderRef colliderRef, Physics::ColliderRef otherColliderRef) noexcept override {}
	void OnCollisionStay(Physics::ColliderRef colliderRef, Physics::ColliderRef otherColliderRef) noexcept override {}
};
#pragma once

#include "Constants.h"
#include "PlayerInputs.h"
#include "Checksum.h"

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
	int BricksLeft = MAX_BRICKS_THAT_CAN_BE_USED;
	float BrickCooldown = COOLDOWN_SPAWN_BRICK;

	bool IsPlayerOnGround = true;
	bool IsPlayerDead = false;

 protected:
	ScreenSizeValue _width{};
	ScreenSizeValue _height{};

	//TODO:
	// Add local input and remote input in ClientGameData, override method to set these inputs, set PlayerRole and add a function to attribute player & ghost inputs
	// Override AddPlayersInputs to set these inputs, change his name to AddInputs(localInput, lastLocalInput, remoteInput, lastRemoteInput)
	// Add ServerGameData with a function to attribute player & ghost inputs, add who is the player and who is the ghost
	// Override switchPlayerAndGhost for ClientGameData and ServerGameData

	// Temporary values used in update, set by SetInputs
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
	 * @brief Set the player inputs, each implementation will know which player has which role to attribute the inputs correctly
	 * @param player1Input
	 * @param player1PreviousInput
	 * @param player2Input
	 * @param player2PreviousInput
	 */
	virtual void SetInputs(PlayerInput player1Input, PlayerInput player1PreviousInput, PlayerInput player2Input, PlayerInput player2PreviousInput) = 0;
	/**
	 * @brief Switch player and ghost, each implementation will know which player has which role to switch the player and ghost
	 */
	virtual void OnSwitchPlayerAndGhost() = 0;

	/**
	 * @brief Register the player inputs, need to be called before Update
	 * @param playerInput
	 * @param previousPlayerInput
	 * @param ghostInput
	 * @param previousGhostInput
	 */
	void AddPlayersInputs(PlayerInput playerInput, PlayerInput previousPlayerInput, PlayerInput ghostInput, PlayerInput previousGhostInput);
	void FixedUpdate();

	void SwitchPlayerAndGhost();

	[[nodiscard]] Math::Vec2F GetGhostPosition() const;

	/**
	 * @brief Compare two GameData, players drawable are not compared
	 * @param other
	 * @return
	 */
	bool operator==(const GameData& other) const;

	[[nodiscard]] Checksum GenerateChecksum() const;

	[[nodiscard]] bool IsGameOver() const;

	void OnTriggerEnter(Physics::ColliderRef colliderRef, Physics::ColliderRef otherColliderRef) noexcept override;
	void OnTriggerExit(Physics::ColliderRef colliderRef, Physics::ColliderRef otherColliderRef) noexcept override;
	void OnTriggerStay(Physics::ColliderRef colliderRef, Physics::ColliderRef otherColliderRef) noexcept override;

	void OnCollisionEnter(Physics::ColliderRef colliderRef, Physics::ColliderRef otherColliderRef) noexcept override;
	void OnCollisionExit(Physics::ColliderRef colliderRef, Physics::ColliderRef otherColliderRef) noexcept override {}
	void OnCollisionStay(Physics::ColliderRef colliderRef, Physics::ColliderRef otherColliderRef) noexcept override {}
};
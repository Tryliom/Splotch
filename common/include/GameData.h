#pragma once

#include "Constants.h"
#include "PlayerInputs.h"

#include "Vec2.h"
#include "World.h"

#include <SFML/System/Time.hpp>

class GameData
{
public:
	Physics::World World;
	Math::Vec2F PlayerPosition;
	HandSlot Hand = HandSlot::SLOT_3; // Not physical, just for the player to know where is the hand

	Physics::BodyRef PlayerBody;

 protected:
	ScreenSizeValue _width{};
	ScreenSizeValue _height{};

	// Temporary values used in update
	PlayerInput _playerInputs{};
	PlayerInput _previousPlayerInputs{};
	PlayerInput _handInputs{};
	PlayerInput _previousHandInputs{};

	// Constants
	static constexpr float PLAYER_SPEED = 400.f;
	static constexpr Math::Vec2F PLAYER_JUMP = GRAVITY * -20.f;

	void DecreaseHandSlot();
	void IncreaseHandSlot();

	[[nodiscard]] Math::Vec2F GetNextPlayerForce(sf::Time elapsed);

	void SetupWorld(Physics::ContactListener* contactListener);

	void UpdatePlayer(sf::Time elapsed);
	void UpdateHand();

 public:
	void StartGame(ScreenSizeValue width, ScreenSizeValue height, Physics::ContactListener* contactListener);

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
};
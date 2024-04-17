#pragma once

#include "Constants.h"
#include "PlayerInputs.h"

#include "Vec2.h"

#include <SFML/System/Time.hpp>

class GameData
{
public:
	Math::Vec2F PlayerPosition;
	HandSlot Hand = HandSlot::SLOT_3;

 protected:
	ScreenSizeValue _width{};
	ScreenSizeValue _height{};

	// Temporary values used in update
	PlayerInput _playerInputs{};
	PlayerInput _previousPlayerInputs{};
	PlayerInput _handInputs{};
	PlayerInput _previousHandInputs{};

	void DecreaseHandSlot();
	void IncreaseHandSlot();

	[[nodiscard]] Math::Vec2F GetNextPlayerPosition(sf::Time elapsed) const;

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
};
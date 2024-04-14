#pragma once

#include "Constants.h"
#include "Packet.h"
#include "Vec2.h"
#include "PlayerInputs.h"
#include "GameData.h"

#include <queue>

class GameManager
{
 public:
	GameManager(ScreenSizeValue width, ScreenSizeValue height);

 private:
	GameData _gameData;

	ScreenSizeValue _width;
	ScreenSizeValue _height;

 public:
	void OnPacketReceived(Packet& packet);

	[[nodiscard]] PlayerInput GetPlayerInputs() const;
	[[nodiscard]] PlayerInput GetHandInputs() const;

	[[nodiscard]] Math::Vec2F GetHandPosition() const;
	void DecreaseHandSlot();
	void IncreaseHandSlot();

	PlayerRole GetPlayerRole();

	void SetPlayerInputs(PlayerInput playerInput, PlayerInput previousPlayerInput);
	void SetHandInputs(PlayerInput playerInput, PlayerInput previousHandInput);

	/**
	 * @brief Calculate the new position of the players using the inputs
	 * @param elapsed Time for a fixed update
	 */
	void UpdatePlayersPositions(sf::Time elapsed);

	void UpdatePlayerAnimations(sf::Time elapsed, sf::Time elapsedSinceLastFixed);

	/**
	 * @brief Get the future position of the player without updating the current position
	 * @param elapsed Time from last fixed update
	 * @return Future position of the player
	 */
	Math::Vec2F GetFuturePlayerPosition(sf::Time elapsed);

	[[nodiscard]] GameData GetGameData() const;
	void SetGameData(GameData gameData);
};
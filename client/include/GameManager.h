#pragma once

#include "Constants.h"
#include "Packet.h"
#include "PlayerInputs.h"
#include "ClientGameData.h"

#include <queue>

/**
 * @brief Manage the game
 */
class GameManager
{
 public:
	/**
	 * @brief Construct a new GameManager object
	 * @param width the width of the screen
	 * @param height the height of the screen
	 */
	GameManager(ScreenSizeValue width, ScreenSizeValue height);

 private:
	/**
	 * @brief The current game data
	 */
	ClientGameData _gameData;

	/**
	 * @brief The width of the screen
	 */
	ScreenSizeValue _width;
	/**
	 * @brief The height of the screen
	 */
	ScreenSizeValue _height;

 public:
	/**
	 * @brief Called when a packet is received
	 * @param packet the packet received
	 */
	void OnPacketReceived(Packet& packet);

	/**
	 * @brief Get the local player role
	 * @return the local player role
	 */
	[[nodiscard]] PlayerRole GetLocalPlayerRole() const;

	/**
	 * @brief Update the game
	 * @param player1Input the input of the player 1
	 * @param player1PreviousInput the previous input of the player 1
	 * @param player2Input the input of the player 2
	 * @param player2PreviousInput the previous input of the player 2
	 */
	void FixedUpdate(PlayerInput player1Input, PlayerInput player1PreviousInput, PlayerInput player2Input, PlayerInput player2PreviousInput);
	/**
	 * @brief Update the player animations
	 * @param elapsed  the elapsed time
	 * @param elapsedSinceLastFixed  the elapsed time since the last fixed update
	 */
	void UpdatePlayerAnimations(sf::Time elapsed, sf::Time elapsedSinceLastFixed);

	/**
	 * @brief Get the game data
	 * @return the game data
	 */
	[[nodiscard]] ClientGameData GetGameData() const;
	/**
	 * @brief Set the game data
	 * @param gameData the game data
	 */
	void SetGameData(ClientGameData gameData);
};
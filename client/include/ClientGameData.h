#pragma once

#include "GameData.h"

#include "PlayerDrawable.h"
#include "Constants.h"

#include <array>

/**
 * @brief ClientGameData is a GameData that is used only on the client side
 */
class ClientGameData final : public GameData
{
 public:
	// Only used in client, not used to compare GameData, first player has always the player role
	std::array<PlayerDrawable, MAX_PLAYERS> Players;

	// Role of the local player
	PlayerRole LocalPlayerRole = PlayerRole::PLAYER;
	// True if the local player is the first player
	bool IsFirstPlayer = true;

	/**
	 * @brief Set the role of the local player
	 * @param playerRole  The role of the local player
	 * @param isFirstPlayer  True if the local player is the first player
	 */
	void SetLocalPlayerRole(PlayerRole playerRole, bool isFirstPlayer);
	/**
	 * @brief Update the animations of the players
	 * @param elapsed  The time elapsed since the last update
	 * @param elapsedSinceLastFixed  The time elapsed since the last fixed update
	 */
	void UpdatePlayersAnimations(sf::Time elapsed, sf::Time elapsedSinceLastFixed);

	/**
	 * @brief Set the inputs for the players, player1 is the local player, player2 is the remote player
	 * @param player1Input
	 * @param player1PreviousInput
	 * @param player2Input
	 * @param player2PreviousInput
	 */
	void SetInputs(PlayerInput player1Input, PlayerInput player1PreviousInput, PlayerInput player2Input, PlayerInput player2PreviousInput) override;

	/**
	 * @brief Called when the player and the ghost are switched, to update the role of the local player
	 */
	void OnSwitchPlayerAndGhost() override;
};
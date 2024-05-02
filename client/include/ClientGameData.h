#pragma once

#include "GameData.h"

#include "PlayerDrawable.h"
#include "Constants.h"

#include <array>

class ClientGameData final : public GameData
{
 public:
	// Only used in client, not used to compare GameData
	std::array<PlayerDrawable, MAX_PLAYERS> Players;

	// Role of the local player
	PlayerRole PlayerRole = PlayerRole::PLAYER;

	void SetPlayersRole(bool isLocalPlayer);
	void UpdatePlayersAnimations(sf::Time elapsed, sf::Time elapsedSinceLastFixed);

	/**
	 * @brief Set the inputs for the players, player1 is the local player, player2 is the remote player
	 * @param player1Input
	 * @param player1PreviousInput
	 * @param player2Input
	 * @param player2PreviousInput
	 */
	void SetInputs(PlayerInput player1Input, PlayerInput player1PreviousInput, PlayerInput player2Input, PlayerInput player2PreviousInput) override;

	void OnSwitchPlayerAndGhost() override;
};
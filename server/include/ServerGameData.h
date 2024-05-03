#pragma once

#include "GameData.h"

class ServerGameData final : public GameData
{
public:
	PlayerRole FirstPlayerRole = PlayerRole::PLAYER;

	void SetFirstPlayerRoles(PlayerRole firstPlayerRole);

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
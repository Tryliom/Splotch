#include "ServerGameData.h"

void ServerGameData::SetFirstPlayerRoles(PlayerRole firstPlayerRole)
{
	FirstPlayerRole = firstPlayerRole;
}

void ServerGameData::SetInputs(PlayerInput player1Input, PlayerInput player1PreviousInput, PlayerInput player2Input, PlayerInput player2PreviousInput)
{
	if (FirstPlayerRole == PlayerRole::PLAYER)
	{
		_playerInputs = player1Input;
		_previousPlayerInputs = player1PreviousInput;
		_ghostInputs = player2Input;
		_previousGhostInputs = player2PreviousInput;
	}
	else
	{
		_playerInputs = player2Input;
		_previousPlayerInputs = player2PreviousInput;
		_ghostInputs = player1Input;
		_previousGhostInputs = player1PreviousInput;
	}
}

void ServerGameData::OnSwitchPlayerAndGhost()
{
	FirstPlayerRole = FirstPlayerRole == PlayerRole::PLAYER ? PlayerRole::GHOST : PlayerRole::PLAYER;
}
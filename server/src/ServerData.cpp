#include "ServerData.h"

#include "Random.h"

namespace ServerData
{
	// Lobby
	bool Lobby::IsFull() const
	{
		return !Players[0].IsEmpty() && !Players[1].IsEmpty();
	}

	bool Lobby::IsEmpty() const
	{
		return Players[0].IsEmpty() && Players[1].IsEmpty();
	}

	bool Lobby::IsInLobby(const ClientId& clientId)
	{
		return Players[0] == clientId || Players[1] == clientId;
	}

	void Lobby::Reset()
	{
		Players = { EMPTY_CLIENT_ID, EMPTY_CLIENT_ID };
	}

	// Game

	Game::Game(const Lobby& lobbyData)
	{
		FromLobby(lobbyData);
	}

	bool Game::IsPlayerInGame(ClientId clientId) const
	{
		return Players[0] == clientId || Players[1] == clientId;
	}

	void Game::Reset()
	{
		Players = { EMPTY_CLIENT_ID, EMPTY_CLIENT_ID };
	}

	void Game::FromLobby(const Lobby& lobbyData)
	{
		Players = lobbyData.Players;

		// Setup players roles, positions etc.
		HandRolePlayer = Math::Random::Range(0, 1);
		PlayerRolePlayer = HandRolePlayer == 0 ? 1 : 0;

		Inputs = { PlayerInputs(), PlayerInputs() };

		RollbackData.clear();
	}
}
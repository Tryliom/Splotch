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

		ConfirmFrames.clear();
	}

	void Game::AddPlayerLastInputs(const std::vector<PlayerInputPerFrame>& inputs, ClientId clientId)
	{
		// Add the inputs to the last inputs according to the frame
		for (const auto& input : inputs)
		{
			if (input.Frame < ConfirmFrames.size() + LastPlayer1Inputs.size()) continue;

			if (clientId == Players[0])
			{
				LastPlayer1Inputs.push_back(input.Input);
			}
			else if (clientId == Players[1])
			{
				LastPlayer2Inputs.push_back(input.Input);
			}
		}
	}

	bool Game::IsNextFrameReady() const
	{
		return LastPlayer1Inputs.size() >= 1 && LastPlayer2Inputs.size() >= 1;
	}

	void Game::AddFrame()
	{
		// Add the frame to the confirmed frames
		ConfirmFrames.push_back({ LastPlayer1Inputs[0], LastPlayer2Inputs[0] });

		// Remove the frame from the last inputs
		LastPlayer1Inputs.erase(LastPlayer1Inputs.begin());
		LastPlayer2Inputs.erase(LastPlayer2Inputs.begin());
	}

	FinalInputs Game::GetLastFrame()
	{
		return ConfirmFrames.back();
	}
}
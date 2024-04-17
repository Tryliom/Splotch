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

		LastGameData.StartGame(WIDTH, HEIGHT, this);

		ConfirmFrames.clear();
	}

	void Game::AddPlayerLastInputs(const std::vector<PlayerInputPerFrame>& inputs, ClientId clientId)
	{
		// Add the inputs to the last inputs according to the frame
		for (const auto& input : inputs)
		{
			if (input.Frame < ConfirmFrames.size()) continue;

			if (clientId == Players[0])
			{
				if (input.Frame < ConfirmFrames.size() + LastPlayer1Inputs.size()) continue;

				LastPlayer1Inputs.push_back(input.Input);
			}
			else if (clientId == Players[1])
			{
				if (input.Frame < ConfirmFrames.size() + LastPlayer2Inputs.size()) continue;

				LastPlayer2Inputs.push_back(input.Input);
			}
		}
	}

	bool Game::IsNextFrameReady() const
	{
		return !LastPlayer1Inputs.empty() && !LastPlayer2Inputs.empty();
	}

	void Game::AddFrame()
	{
		// Add the frame to the confirmed frames
		if (PlayerRolePlayer == 0)
		{
			ConfirmFrames.push_back({ LastPlayer1Inputs[0], LastPlayer2Inputs[0] });
		}
		else
		{
			ConfirmFrames.push_back({ LastPlayer2Inputs[0], LastPlayer1Inputs[0] });
		}

		// Remove the frame from the last inputs
		LastPlayer1Inputs.erase(LastPlayer1Inputs.begin());
		LastPlayer2Inputs.erase(LastPlayer2Inputs.begin());

		// Update the game data
		const auto confirmedFrameSize = ConfirmFrames.size();
		const auto playerInputs = ConfirmFrames[confirmedFrameSize - 1].PlayerRoleInputs;
		const auto previousPlayerInputs = confirmedFrameSize > 1 ? ConfirmFrames[confirmedFrameSize - 2].PlayerRoleInputs : playerInputs;
		const auto handInputs = ConfirmFrames[confirmedFrameSize - 1].HandRoleInputs;
		const auto previousHandInputs = confirmedFrameSize > 1 ? ConfirmFrames[confirmedFrameSize - 2].HandRoleInputs : handInputs;

		LastGameData.RegisterPlayersInputs(playerInputs, previousPlayerInputs, handInputs, previousHandInputs);
		LastGameData.Update(sf::seconds(TIME_PER_FRAME));
	}

	FinalInputs Game::GetLastFrame()
	{
		return ConfirmFrames.back();
	}
}
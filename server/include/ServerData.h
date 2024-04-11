#pragma once

#include "Constants.h"
#include "ClientId.h"
#include "PlayerInputs.h"

#include <array>
#include <vector>

namespace ServerData
{
	struct FinalInputs
	{
		PlayerInput PlayerRoleInputs;
		PlayerInput HandRoleInputs;
	};

	struct Lobby
	{
		std::array<ClientId, 2> Players = { EMPTY_CLIENT_ID, EMPTY_CLIENT_ID };

		[[nodiscard]] bool IsFull() const;
		[[nodiscard]] bool IsEmpty() const;
		[[nodiscard]] bool IsInLobby(const ClientId& clientId);

		void Reset();
	};

	struct Game
	{
		std::array<ClientId, 2> Players = { EMPTY_CLIENT_ID, EMPTY_CLIENT_ID };

		// Index of the player who is the hand role player
		int HandRolePlayer{};
		// Index of the player who is the point role player
		int PlayerRolePlayer{};

		// Confirmed frame inputs
		std::vector<FinalInputs> ConfirmFrames;

		std::vector<PlayerInput> LastPlayer1Inputs;
		std::vector<PlayerInput> LastPlayer2Inputs;

		explicit Game(const Lobby& lobbyData);

		[[nodiscard]] bool IsPlayerInGame(ClientId clientId) const;

		void Reset();
		void FromLobby(const Lobby& lobbyData);

		void AddPlayerLastInputs(const std::vector<PlayerInputPerFrame>& inputs, ClientId clientId);

		[[nodiscard]] bool IsNextFrameReady() const;
		void AddFrame();
		FinalInputs GetLastFrame();

	};
}
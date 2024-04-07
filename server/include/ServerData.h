#pragma once

#include "Constants.h"
#include "ClientId.h"
#include "PlayerInputs.h"

#include <array>
#include <vector>

namespace ServerData
{
	struct RollbackInfo
	{
		PlayerInputs PlayerRoleInputs;
		PlayerInputs HandRoleInputs;
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
		std::array<PlayerInputs, 2> Inputs;

		// Index of the player who is the hand role player
		int HandRolePlayer;
		// Index of the player who is the point role player
		int PlayerRolePlayer;

		// Rollback data per confirmed frame
		std::vector<RollbackInfo> RollbackData;

		explicit Game(const Lobby& lobbyData);

		[[nodiscard]] bool IsPlayerInGame(ClientId clientId) const;

		void Reset();
		void FromLobby(const Lobby& lobbyData);
	};
}
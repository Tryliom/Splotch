#pragma once

#include "Constants.h"
#include "ClientId.h"
#include "PlayerName.h"
#include "MyPackets/LobbyInformationPacket.h"

#include <array>
#include <vector>

namespace ServerData
{
	struct Lobby
	{
		std::array<ClientId, 2> Players = { EMPTY_CLIENT_ID, EMPTY_CLIENT_ID };
		std::array<PlayerName, 2> PlayerNames = { PlayerName{}, PlayerName{} };
		std::array<IconType, 2> PlayerIcons = { IconType::Icon1, IconType::Icon1 };
		DeckType ChosenDeckType = DeckType::Deck3x2;

		[[nodiscard]] bool IsFull() const;
		[[nodiscard]] bool IsEmpty() const;
		[[nodiscard]] bool IsInLobby(const ClientId& clientId);

		[[nodiscard]] MyPackets::LobbyInformationPacket* ToPacket(bool isHost) const;

		void Reset();
	};

	struct Game
	{
		std::array<ClientId, 2> Players = { EMPTY_CLIENT_ID, EMPTY_CLIENT_ID };
		std::vector<CardIconIndex> Cards;

		char CurrentTurn{};
		std::array<CardIndex, 2> SelectedCards{};
		std::array<char, 2> Scores{};

		explicit Game(const Lobby& lobbyData);

		void SelectCard(CardIndex index);
		void UnselectCards();

		[[nodiscard]] bool HasSelectedTwoCards() const;
		[[nodiscard]] bool IsGameOver() const;
		[[nodiscard]] bool IsPlayerInGame(ClientId clientId) const;

		void Reset();
		void FromLobby(const Lobby& lobbyData);
	};
}
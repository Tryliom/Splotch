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

	MyPackets::LobbyInformationPacket* Lobby::ToPacket(bool isHost) const
	{
		return new MyPackets::LobbyInformationPacket(isHost, !IsFull(),
			PlayerNames[0], PlayerNames[1], PlayerIcons[0], PlayerIcons[1], ChosenDeckType);
	}

	void Lobby::Reset()
	{
		Players = { EMPTY_CLIENT_ID, EMPTY_CLIENT_ID };
		PlayerNames = { PlayerName{}, PlayerName{} };
		PlayerIcons = { IconType::Icon1, IconType::Icon1 };
		ChosenDeckType = DeckType::Deck3x2;
	}

	// Game

	Game::Game(const Lobby& lobbyData)
	{
		FromLobby(lobbyData);
	}

	bool Game::HasSelectedTwoCards() const
	{
		return SelectedCards[0] != UNKNOWN_CARD_INDEX && SelectedCards[1] != UNKNOWN_CARD_INDEX;
	}

	void Game::SelectCard(CardIndex index)
	{
		for (auto& selectedCard : SelectedCards)
		{
			if (selectedCard == index) return;
			if (selectedCard != UNKNOWN_CARD_INDEX) continue;

			selectedCard = index;
			return;
		}
	}

	void Game::UnselectCards()
	{
		SelectedCards[0] = UNKNOWN_CARD_INDEX;
		SelectedCards[1] = UNKNOWN_CARD_INDEX;
	}

	bool Game::IsGameOver() const
	{
		return Scores[0] + Scores[1] == Cards.size() / 2;
	}

	bool Game::IsPlayerInGame(ClientId clientId) const
	{
		return Players[0] == clientId || Players[1] == clientId;
	}

	void Game::Reset()
	{
		Players = { EMPTY_CLIENT_ID, EMPTY_CLIENT_ID };
		Cards.clear();
		CurrentTurn = 0;
		SelectedCards = { UNKNOWN_CARD_INDEX, UNKNOWN_CARD_INDEX };
		Scores = { 0, 0 };
	}

	void Game::FromLobby(const Lobby& lobbyData)
	{
		Players = lobbyData.Players;

		// Create deck
		std::size_t cardCount = 0;
		switch (lobbyData.ChosenDeckType)
		{
		case DeckType::Deck3x2:
			cardCount = 6;
			break;
		case DeckType::Deck7x2:
			cardCount = 14;
			break;
		case DeckType::Deck6x5:
			cardCount = 30;
			break;
		case DeckType::Deck7x6:
			cardCount = 42;
			break;
		case DeckType::Deck10x5:
			cardCount = 50;
			break;
		}

		const auto& maxCard = cardCount / 2;
		for (char i = 0; i < maxCard; i++)
		{
			Cards.push_back(CardIconIndex { i });
			Cards.push_back(CardIconIndex { i });
		}

		Random::Shuffle(Cards);

		CurrentTurn = static_cast<char>(Random::Range(0, 1));
		SelectedCards = { UNKNOWN_CARD_INDEX, UNKNOWN_CARD_INDEX };
	}
}
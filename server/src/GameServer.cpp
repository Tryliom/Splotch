#include "GameServer.h"

#include "Logger.h"
#include "Constants.h"

#include "MyPackets/StartGamePacket.h"
#include "MyPackets/LeaveGamePacket.h"
#include "MyPackets/JoinLobbyPacket.h"

Server::Server(ServerNetworkInterface& serverNetworkInterface) : _serverNetworkInterface(serverNetworkInterface)
{
}

void Server::Update()
{
	bool processPackets = true;
	while (processPackets)
	{
		auto packetData = _serverNetworkInterface.PopPacket();

		if (packetData.Packet == nullptr)
		{
			processPackets = false;
			break;
		}

		OnReceivePacket(packetData);

		delete packetData.Packet;
	}

	bool processDisconnectedClients = true;
	while (processDisconnectedClients)
	{
		auto clientId = _serverNetworkInterface.PopDisconnectedClient();

		if (clientId == EMPTY_CLIENT_ID)
		{
			processDisconnectedClients = false;
			break;
		}

		OnDisconnect(clientId);
	}
}

void Server::OnReceivePacket(PacketData packetData)
{
	auto clientId = packetData.Client;
	auto packet = packetData.Packet;

	if (packet->Type == static_cast<char>(MyPackets::MyPacketType::JoinLobby))
	{
		const auto* joinLobbyPacket = packet->As<MyPackets::JoinLobbyPacket>();
		LOG("Player " << clientId.Index << " aka " << joinLobbyPacket->Name.AsString() << " joined the lobby");
		JoinLobby(clientId, joinLobbyPacket->Name, joinLobbyPacket->IconIndex);
	}
	else if (packet->Type == static_cast<char>(MyPackets::MyPacketType::ChangeDeck))
	{
		const auto* changeDeckPacket = packet->As<MyPackets::ChangeDeckPacket>();

		ChangeDeck(clientId, changeDeckPacket->ChosenDeckType);
	}
	else if (packet->Type == static_cast<char>(MyPackets::MyPacketType::LeaveLobby))
	{
		LOG("Player " << clientId.Index << " left the lobby");
		RemoveFromLobby(clientId);
	}
	else if (packet->Type == static_cast<char>(MyPackets::MyPacketType::StartGame))
	{
		LOG("Player " << clientId.Index << " started the game");
		StartGame(clientId);
	}
	else if (packet->Type == static_cast<char>(MyPackets::MyPacketType::CardInformation))
	{
		const auto* selectCardPacket = packet->As<MyPackets::CardInformationPacket>();

		SelectCard(clientId, selectCardPacket->CardIndexInDeck);
	}
	else if (packet->Type == static_cast<char>(MyPackets::MyPacketType::LeaveGame))
	{
		LOG("Player " << clientId.Index << " left the game");
		RemoveFromGame(clientId);
	}
}

void Server::OnDisconnect(ClientId clientId)
{
	LOG("Player " << clientId.Index << " disconnected");
	RemoveFromLobby(clientId);
	RemoveFromGame(clientId);
}

void Server::JoinLobby(ClientId clientId, PlayerName playerName, IconType iconType)
{
	static constexpr char FIRST_PLAYER_INDEX = 0;
	static constexpr char SECOND_PLAYER_INDEX = 1;

	// Check if there is a lobby with only one player
	for (auto& lobby: _lobbies)
	{
		if (lobby.Players[FIRST_PLAYER_INDEX] == EMPTY_CLIENT_ID || lobby.Players[SECOND_PLAYER_INDEX] == EMPTY_CLIENT_ID)
		{
			AddToLobby(lobby, clientId, playerName, iconType);

			return;
		}
	}

	// If there is no lobby with only one player, create a new lobby
	_lobbies.emplace_back();
	AddToLobby(_lobbies.back(), clientId, playerName, iconType);
}

void Server::AddToLobby(ServerData::Lobby& lobby, ClientId clientId, PlayerName playerName, IconType iconType)
{
	static constexpr char FIRST_PLAYER_INDEX = 0;
	static constexpr char SECOND_PLAYER_INDEX = 1;

	if (lobby.Players[FIRST_PLAYER_INDEX] == EMPTY_CLIENT_ID)
	{
		lobby.Players[FIRST_PLAYER_INDEX] = clientId;
		lobby.PlayerNames[FIRST_PLAYER_INDEX] = playerName;
		lobby.PlayerIcons[FIRST_PLAYER_INDEX] = iconType;

		_serverNetworkInterface.SendPacket(lobby.ToPacket(true), clientId);
	}
	else if (lobby.Players[SECOND_PLAYER_INDEX] == EMPTY_CLIENT_ID)
	{
		lobby.Players[SECOND_PLAYER_INDEX] = clientId;
		lobby.PlayerNames[SECOND_PLAYER_INDEX] = playerName;
		lobby.PlayerIcons[SECOND_PLAYER_INDEX] = iconType;

		_serverNetworkInterface.SendPacket(lobby.ToPacket(true), lobby.Players[FIRST_PLAYER_INDEX]);
		_serverNetworkInterface.SendPacket(lobby.ToPacket(false), clientId);
	}
}

void Server::RemoveFromLobby(ClientId clientId)
{
	static constexpr char FIRST_PLAYER_INDEX = 0;
	static constexpr char SECOND_PLAYER_INDEX = 1;

	// Remove the player from the lobby
	for (auto& lobby: _lobbies)
	{
		if (lobby.Players[FIRST_PLAYER_INDEX] == clientId)
		{
			if (lobby.Players[SECOND_PLAYER_INDEX] != EMPTY_CLIENT_ID)
			{
				auto player = lobby.Players[SECOND_PLAYER_INDEX];
				auto name = lobby.PlayerNames[SECOND_PLAYER_INDEX];
				auto iconIndex = lobby.PlayerIcons[SECOND_PLAYER_INDEX];

				lobby.Reset();

				JoinLobby(player, name, iconIndex);
			}
			else
			{
				lobby.Reset();
			}
			break;
		}
		else if (lobby.Players[SECOND_PLAYER_INDEX] == clientId)
		{
			lobby.Players[SECOND_PLAYER_INDEX] = EMPTY_CLIENT_ID;

			auto player = lobby.Players[FIRST_PLAYER_INDEX];
			auto name = lobby.PlayerNames[FIRST_PLAYER_INDEX];
			auto iconIndex = lobby.PlayerIcons[FIRST_PLAYER_INDEX];

			lobby.Reset();

			JoinLobby(player, name, iconIndex);

			break;
		}
	}
}

void Server::RemoveFromGame(ClientId clientId)
{
	static constexpr char FIRST_PLAYER_INDEX = 0;
	static constexpr char SECOND_PLAYER_INDEX = 1;

	// Remove the player from the game
	for (auto i = 0; i < _games.size(); i++)
	{
		auto& game = _games[i];

		if (!game.IsPlayerInGame(clientId)) continue;

		// Send a message to the other player that the opponent left the game
		const auto& opponent = game.Players[FIRST_PLAYER_INDEX] == clientId ? game.Players[SECOND_PLAYER_INDEX] : game.Players[FIRST_PLAYER_INDEX];
		_serverNetworkInterface.SendPacket(new MyPackets::LeaveGamePacket(), opponent);

		game.Reset();

		return;
	}
}

void Server::ChangeDeck(ClientId clientId, DeckType deckType)
{
	// Find the lobby with the player
	for (auto& lobby: _lobbies)
	{
		if (lobby.Players[0] == clientId) // Only host can change deck
		{
			lobby.ChosenDeckType = deckType;
			break;
		}
	}
}

void Server::StartGame(ClientId clientId)
{
	// Find the lobby with the player
	for (auto& lobby: _lobbies)
	{
		if (lobby.IsEmpty()) continue;
		if (!lobby.IsInLobby(clientId)) continue;
		if (lobby.Players[0] != clientId) return; // Only host can start the game

		// Find or create a game
		for (auto& game: _games)
		{
			if (game.Players[0] == EMPTY_CLIENT_ID || game.Players[1] == EMPTY_CLIENT_ID)
			{
				StartNewGame(game, lobby);
				return;
			}
		}

		_games.emplace_back(lobby);
		auto& game = _games.back();

		StartNewGame(game, lobby);
		return;
	}
}

void Server::StartNewGame(ServerData::Game& game, ServerData::Lobby& lobby)
{
	game.Reset();
	game.FromLobby(lobby);

	// Send a message to the players that the game is starting
	_serverNetworkInterface.SendPacket(
		new MyPackets::StartGamePacket(lobby.ChosenDeckType, game.CurrentTurn == 0), lobby.Players[0]);
	_serverNetworkInterface.SendPacket(
		new MyPackets::StartGamePacket(lobby.ChosenDeckType, game.CurrentTurn == 1), lobby.Players[1]);

	// Remove the lobby
	lobby.Reset();
}

void Server::SelectCard(ClientId clientId, CardIndex cardIndex)
{
	static constexpr char PLAYER1_INDEX = 0;
	static constexpr char PLAYER2_INDEX = 1;
	static constexpr char FIRST_SELECTED_CARD = 0;
	static constexpr char SECOND_SELECTED_CARD = 1;

	// Find the game with the player
	for (auto i = 0; i < _games.size(); i++)
	{
		auto& game = _games[i];

		if (!game.IsPlayerInGame(clientId)) continue;

		auto playerTurn = game.CurrentTurn;
		auto requestPlayer = game.Players[PLAYER1_INDEX] == clientId ? PLAYER1_INDEX : PLAYER2_INDEX;

		if (playerTurn != requestPlayer) return;
		if (game.HasSelectedTwoCards()) return;
		if (game.SelectedCards[FIRST_SELECTED_CARD] == cardIndex) return;

		game.SelectCard(cardIndex);

		const auto iconIndex = game.Cards[cardIndex.Index];

		for (const auto& player: game.Players)
		{
			_serverNetworkInterface.SendPacket(new MyPackets::CardInformationPacket(cardIndex, iconIndex), player);
		}

		if (!game.HasSelectedTwoCards()) return;

		if (game.Cards[game.SelectedCards[FIRST_SELECTED_CARD].Index] !=
			game.Cards[game.SelectedCards[SECOND_SELECTED_CARD].Index])
		{
			game.CurrentTurn = game.CurrentTurn == PLAYER1_INDEX ? PLAYER2_INDEX : PLAYER1_INDEX;
		}
		else
		{
			game.Scores[game.CurrentTurn]++;
		}

		game.UnselectCards();

		if (game.IsGameOver())
		{
			game.Reset();
			return;
		}

		// Send a message to the players whose turn it is
		for (const auto& player: game.Players)
		{
			_serverNetworkInterface.SendPacket(new MyPackets::TurnPacket(game.CurrentTurn == player.Index), player);
		}
	}
}
#include "GameServer.h"

#include "Logger.h"
#include "Constants.h"

#include "MyPackets/StartGamePacket.h"
#include "MyPackets/LeaveGamePacket.h"
#include "MyPackets/PlayerInputPacket.h"
#include "MyPackets/ConfirmationInputPacket.h"

GameServer::GameServer(ServerNetworkInterface& serverNetworkInterface)
	: _serverNetworkInterface(serverNetworkInterface) {}

void GameServer::Update()
{
	while (true)
	{
		auto packetData = _serverNetworkInterface.PopPacket();

		if (packetData.PacketContent == nullptr) break;

		OnReceivePacket(packetData);

		delete packetData.PacketContent;
	}

	while (true)
	{
		auto clientId = _serverNetworkInterface.PopDisconnectedClient();

		if (clientId == EMPTY_CLIENT_ID) break;

		OnDisconnect(clientId);
	}

	for (auto& game: _games)
	{
		while (game.IsNextFrameReady())
		{
			game.AddFrame();

			const auto frame = game.GetLastFrame();
			const auto checksum = game.LastGameData.GenerateChecksum();

			const auto p1ConfirmedPacket = new MyPackets::ConfirmInputPacket(frame.PlayerRoleInputs, frame.HandRoleInputs, checksum);
			const auto p2ConfirmedPacket = new MyPackets::ConfirmInputPacket(frame.PlayerRoleInputs, frame.HandRoleInputs, checksum);

			// Send the frame to the players
			_serverNetworkInterface.SendPacket(p1ConfirmedPacket, game.Players[0], Protocol::TCP);
			_serverNetworkInterface.SendPacket(p2ConfirmedPacket, game.Players[1], Protocol::TCP);
		}
	}
}

void GameServer::OnReceivePacket(PacketData packetData)
{
	auto clientId = packetData.Client;
	auto packet = packetData.PacketContent;

	if (packet->Type == static_cast<char>(MyPackets::MyPacketType::JoinLobby))
	{
		LOG("PlayerDrawable " << clientId.Index << " joined the lobby");
		JoinLobby(clientId);
	}
	else if (packet->Type == static_cast<char>(MyPackets::MyPacketType::LeaveLobby))
	{
		LOG("PlayerDrawable " << clientId.Index << " left the lobby");
		RemoveFromLobby(clientId);
	}
	else if (packet->Type == static_cast<char>(MyPackets::MyPacketType::StartGame))
	{
		LOG("PlayerDrawable " << clientId.Index << " started the game");
		StartGame(clientId);
	}
	else if (packet->Type == static_cast<char>(MyPackets::MyPacketType::LeaveGame))
	{
		LOG("PlayerDrawable " << clientId.Index << " left the game");
		RemoveFromGame(clientId);
	}
	else if (packet->Type == static_cast<char>(MyPackets::MyPacketType::PlayerInput))
	{
		// Forward the packet to the game
		for (auto& game: _games)
		{
			if (game.IsPlayerInGame(clientId))
			{
				const auto otherClientId = game.Players[0] == clientId ? game.Players[1] : game.Players[0];
				game.AddPlayerLastInputs(packet->As<MyPackets::PlayerInputPacket>()->LastInputs, clientId);
				// Send input to other player
				_serverNetworkInterface.SendPacket(packet->Clone(), otherClientId, Protocol::UDP);
				break;
			}
		}
	}
}

void GameServer::OnDisconnect(ClientId clientId)
{
	LOG("PlayerDrawable " << clientId.Index << " disconnected");
	RemoveFromLobby(clientId);
	RemoveFromGame(clientId);
}

void GameServer::JoinLobby(ClientId clientId)
{
	static constexpr char FIRST_PLAYER_INDEX = 0;
	static constexpr char SECOND_PLAYER_INDEX = 1;

	// Check if there is a lobby with only one player
	for (auto& lobby: _lobbies)
	{
		if (lobby.Players[FIRST_PLAYER_INDEX] == EMPTY_CLIENT_ID || lobby.Players[SECOND_PLAYER_INDEX] == EMPTY_CLIENT_ID)
		{
			AddToLobby(lobby, clientId);

			return;
		}
	}

	// If there is no lobby with only one player, create a new lobby
	_lobbies.emplace_back();
	AddToLobby(_lobbies.back(), clientId);
}

void GameServer::AddToLobby(ServerData::Lobby& lobby, ClientId clientId)
{
	static constexpr char FIRST_PLAYER_INDEX = 0;
	static constexpr char SECOND_PLAYER_INDEX = 1;

	if (lobby.Players[FIRST_PLAYER_INDEX] == EMPTY_CLIENT_ID)
	{
		lobby.Players[FIRST_PLAYER_INDEX] = clientId;
	}
	else if (lobby.Players[SECOND_PLAYER_INDEX] == EMPTY_CLIENT_ID)
	{
		lobby.Players[SECOND_PLAYER_INDEX] = clientId;

		StartGame(lobby.Players[FIRST_PLAYER_INDEX]);
	}
}

void GameServer::RemoveFromLobby(ClientId clientId)
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

				lobby.Reset();

				JoinLobby(player);
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

			lobby.Reset();

			JoinLobby(player);

			break;
		}
	}
}

void GameServer::RemoveFromGame(ClientId clientId)
{
	static constexpr char FIRST_PLAYER_INDEX = 0;
	static constexpr char SECOND_PLAYER_INDEX = 1;

	// Remove the player from the game
	for (auto & game : _games)
	{
		if (!game.IsPlayerInGame(clientId)) continue;

		// Send a message to the other player that the opponent left the game
		const auto& opponent = game.Players[FIRST_PLAYER_INDEX] == clientId ? game.Players[SECOND_PLAYER_INDEX] : game.Players[FIRST_PLAYER_INDEX];
		_serverNetworkInterface.SendPacket(new MyPackets::LeaveGamePacket(), opponent, Protocol::TCP);

		game.Reset();

		return;
	}
}

void GameServer::StartGame(ClientId clientId)
{
	// Find the lobby with the player
	for (auto& lobby: _lobbies)
	{
		if (lobby.IsEmpty()) continue;
		if (!lobby.IsInLobby(clientId)) continue;

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

void GameServer::StartNewGame(ServerData::Game& game, ServerData::Lobby& lobby)
{
	game.Reset();
	game.FromLobby(lobby);

	// Send a message to the players that the game is starting
	_serverNetworkInterface.SendPacket(new MyPackets::StartGamePacket(game.PlayerRolePlayer == 0), lobby.Players[0], Protocol::TCP);
	_serverNetworkInterface.SendPacket(new MyPackets::StartGamePacket(game.PlayerRolePlayer == 1), lobby.Players[1], Protocol::TCP);

	// Remove the lobby
	lobby.Reset();
}
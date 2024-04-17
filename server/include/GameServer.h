#pragma once

#include "Constants.h"
#include "NetworkServerManager.h"
#include "ServerData.h"

#include <SFML/Network.hpp>

#include <array>

class GameServer
{
public:
	explicit GameServer(ServerNetworkInterface& serverNetworkInterface);

private:
	std::vector<ServerData::Lobby> _lobbies;
	std::vector<ServerData::Game> _games;

	ServerNetworkInterface& _serverNetworkInterface;

	void OnReceivePacket(PacketData packetData);
	void OnDisconnect(ClientId clientId);

public:
	void Update();

private:
	void JoinLobby(ClientId clientId);
	void AddToLobby(ServerData::Lobby& lobby, ClientId clientId);
	void RemoveFromLobby(ClientId clientId);
	void RemoveFromGame(ClientId clientId);
	void StartGame(ClientId clientId);
	void StartNewGame(ServerData::Game& game, ServerData::Lobby& lobby);
};
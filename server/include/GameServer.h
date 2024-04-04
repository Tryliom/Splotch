#pragma once

#include "Constants.h"
#include "NetworkServerManager.h"
#include "ServerData.h"

#include <SFML/Network.hpp>

#include <array>

class Server
{
public:
	explicit Server(ServerNetworkInterface& serverNetworkInterface);

private:
	std::vector<ServerData::Lobby> _lobbies;
	std::vector<ServerData::Game> _games;

	ServerNetworkInterface& _serverNetworkInterface;

	void OnReceivePacket(PacketData packetData);
	void OnDisconnect(ClientId clientId);

public:
	void Update();

private:
	void JoinLobby(ClientId clientId, PlayerName playerName, IconType iconType);
	void AddToLobby(ServerData::Lobby& lobby, ClientId clientId, PlayerName playerName, IconType iconType);
	void RemoveFromLobby(ClientId clientId);
	void RemoveFromGame(ClientId clientId);
	void ChangeDeck(ClientId clientId, DeckType deckType);
	void StartGame(ClientId clientId);
	void StartNewGame(ServerData::Game& game, ServerData::Lobby& lobby);
	void SelectCard(ClientId clientId, CardIndex cardIndex);
};
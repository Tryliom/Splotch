#pragma once

#include "PacketManager.h"
#include "ServerNetworkInterface.h"

#include <atomic>
#include <functional>
#include <queue>
#include <mutex>
#include <array>

struct UDPClient
{
	sf::IpAddress Address;
	unsigned short Port;
};

class NetworkServerManager final : public ServerNetworkInterface
{
private:
	static constexpr char MAX_CLIENTS = 100;

	std::queue<PacketData> _packetsToProcess;
	mutable std::mutex _mutexToProcessPackets;

	std::queue<PacketData> _packetsToSend;
	mutable std::mutex _mutexToSendPackets;

	std::queue<ClientId> _disconnectedClients;
	mutable std::mutex _mutexDisconnectedClients;

	std::array<std::pair<ClientId, sf::TcpSocket*>, MAX_CLIENTS> _clients;
	mutable std::mutex _mutexClients;

	std::array<std::pair<ClientId, UDPClient>, MAX_CLIENTS> _udpClients;
	mutable std::mutex _mutexUdpClients;

	sf::TcpListener _tcpListener;
	sf::UdpSocket _udpSocket;

public:
	explicit NetworkServerManager(unsigned short port);
	~NetworkServerManager();

	std::atomic<bool> Running = true;

	PacketData PopPacket() override;
	void SendPacket(Packet* packet, const ClientId& clientId, Protocol protocol) override;
	ClientId PopDisconnectedClient() override;

private:
	ClientId AddClient(sf::TcpSocket* socket);

	void AcceptNewClients();
	void ReceivePacketFromClient(const ClientId& clientId);
	void ReceivePacketFromUdp();
};
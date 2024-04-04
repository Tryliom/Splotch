#pragma once

#include "Packet.h"
#include "ClientNetworkInterface.h"

#include <shared_mutex>
#include <queue>

/**
 * Receive and send packet from/to server
 */
class NetworkClientManager final : public ClientNetworkInterface
{
private:
	sf::TcpSocket* _socket = new sf::TcpSocket();
	std::queue<Packet*> _packetReceived;
	mutable std::shared_mutex _receivedMutex;
	std::queue<Packet*> _packetToSend;
	mutable std::shared_mutex _sendMutex;
	bool _running = true;

	// Launch from a thread
	void ReceivePackets();
	void SendPackets();

	bool IsPacketReceivedEmpty() const
	{
		std::shared_lock lock(_receivedMutex);
		return _packetReceived.empty();
	}

	bool IsPacketToSendEmpty() const
	{
		std::shared_lock lock(_sendMutex);
		return _packetToSend.empty();
	}

public:
	NetworkClientManager(std::string_view host, unsigned short port);

	Packet* PopPacket() override;
	void SendPacket(Packet* packet) override;

	void Stop();
};
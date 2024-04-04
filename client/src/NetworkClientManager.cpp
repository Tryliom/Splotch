#include "NetworkClientManager.h"

#include "PacketManager.h"
#include "Logger.h"

#include <thread>

NetworkClientManager::NetworkClientManager(std::string_view host, unsigned short port)
{
	_running = true;
	_socket = new sf::TcpSocket();

	if (_socket->connect(host.data(), port) != sf::Socket::Done)
	{
		LOG_ERROR("Could not connect to server");
		std::exit(EXIT_FAILURE);
	}

	std::thread receiveThread(&NetworkClientManager::ReceivePackets, this);
	receiveThread.detach();

	std::thread sendThread(&NetworkClientManager::SendPackets, this);
	sendThread.detach();
}

void NetworkClientManager::ReceivePackets()
{
	while (_running)
	{
		Packet* packet = PacketManager::ReceivePacket(*_socket);

		if (packet->Type == static_cast<char>(PacketType::Invalid))
		{
			LOG_ERROR("Could not receive packet");
			std::exit(EXIT_FAILURE);
		}

		std::scoped_lock lock(_receivedMutex);
		_packetReceived.push(packet);
	}
}

void NetworkClientManager::SendPackets()
{
	while (_running)
	{
		if (IsPacketToSendEmpty()) continue;

		std::scoped_lock lock(_sendMutex);

		auto* packet = _packetToSend.front();
		auto* sfPacket = PacketManager::ToSfPacket(packet);

		_packetToSend.pop();
		_socket->send(*sfPacket);

		delete packet;
		delete sfPacket;
	}
}

Packet* NetworkClientManager::PopPacket()
{
	if (IsPacketReceivedEmpty()) return nullptr;

	std::scoped_lock lock(_receivedMutex);
	if (_packetReceived.empty()) return nullptr;

	auto* packet = _packetReceived.front();
	_packetReceived.pop();

	return packet;
}

void NetworkClientManager::SendPacket(Packet* packet)
{
	std::scoped_lock lock(_sendMutex);
	_packetToSend.push(packet);
}

void NetworkClientManager::Stop()
{
	_running = false;
	_socket->disconnect();
	delete _socket;
}
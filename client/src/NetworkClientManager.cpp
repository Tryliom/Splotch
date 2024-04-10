#include "NetworkClientManager.h"

#include "PacketManager.h"
#include "Logger.h"

#include <thread>

NetworkClientManager::NetworkClientManager(std::string_view host, unsigned short port)
{
	_running = true;
	_socket = new sf::TcpSocket();

	_udpSocket.setBlocking(false);

	if (_socket->connect(host.data(), port) != sf::Socket::Done)
	{
		LOG_ERROR("Could not connect to server");
		std::exit(EXIT_FAILURE);
	}

	UdpPort = port + 1;

	std::thread receiveThread(&NetworkClientManager::ReceivePackets, this);
	receiveThread.detach();

	std::thread sendThread(&NetworkClientManager::SendPackets, this);
	sendThread.detach();

	std::thread receiveUdpThread(&NetworkClientManager::ReceiveUDPPackets, this);
	receiveUdpThread.detach();
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
		while (!IsPacketToSendEmpty())
		{
			std::scoped_lock lock(_sendMutex);

			auto packetProtocol = _packetToSend.front();
			auto* packet = packetProtocol.Packet;
			auto protocol = packetProtocol.Protocol;
			auto* sfPacket = PacketManager::ToSfPacket(packet);

			_packetToSend.pop();

			if (protocol == Protocol::TCP)
			{
				_socket->send(*sfPacket);
			}
			else
			{
				_udpSocket.send(*sfPacket, _socket->getRemoteAddress(), UdpPort);
			}

			delete packet;
			delete sfPacket;
		}
	}
}

void NetworkClientManager::ReceiveUDPPackets()
{
	sf::IpAddress sender;
	unsigned short port;
	sf::Packet sfPacket;
	if (_udpSocket.receive(sfPacket, sender, port) == sf::Socket::Done)
	{
		auto* packet = PacketManager::FromPacket(&sfPacket);
		std::scoped_lock lock(_receivedMutex);
		_packetReceived.push(packet);
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

void NetworkClientManager::SendPacket(Packet* packet, Protocol protocol)
{
	std::scoped_lock lock(_sendMutex);
	_packetToSend.push({packet, protocol});
}

void NetworkClientManager::SendUDPAcknowledgmentPacket()
{
	std::scoped_lock lock(_sendMutex);
	auto* packet = new UDPAcknowledgePacket(_socket->getLocalPort());

	_packetToSend.push({packet, Protocol::UDP});
}

void NetworkClientManager::Stop()
{
	_running = false;
	_socket->disconnect();
	delete _socket;
}
#include "NetworkServerManager.h"

#include "Logger.h"

#include <thread>
#include <utility>

NetworkServerManager::NetworkServerManager(unsigned short port)
{
	Running = true;

	if (_tcpListener.listen(port) != sf::Socket::Done)
	{
		LOG_ERROR("Could not bind to TCP port " << port);
		std::exit(EXIT_FAILURE);
	}

	if (_udpSocket.bind(port + 1) != sf::Socket::Done)
	{
		LOG_ERROR("Could not bind to UDP port " << port);
		std::exit(EXIT_FAILURE);
	}

	_udpSocket.setBlocking(false);

	LOG("Server is listening to port " << port);

	std::thread clientAcceptor = std::thread([this]() {AcceptNewClients();});
	clientAcceptor.detach();

	std::thread udpReceiver = std::thread([this]() {ReceivePacketFromUdp();});
	udpReceiver.detach();
}

NetworkServerManager::~NetworkServerManager()
{
	Running = false;
	_tcpListener.close();
}

PacketData NetworkServerManager::PopPacket()
{
	std::scoped_lock lock(_mutexToProcessPackets);

	if (_packetsToProcess.empty()) return PacketData { nullptr, EMPTY_CLIENT_ID };

	auto packetData = _packetsToProcess.front();
	_packetsToProcess.pop();

	return packetData;
}

void NetworkServerManager::SendPacket(Packet* packet, const ClientId& clientId, Protocol protocol)
{
	auto* sfPacket = PacketManager::ToSfPacket(packet);

	if (protocol == Protocol::TCP)
	{
		_clients[clientId.Index].second->send(*sfPacket);
	}
	else
	{
		_udpSocket.send(*sfPacket, _udpClients[clientId.Index].second.Address, _udpClients[clientId.Index].second.Port);
	}

	delete packet;
	delete sfPacket;
}

ClientId NetworkServerManager::PopDisconnectedClient()
{
	std::scoped_lock lock(_mutexDisconnectedClients);

	if (_disconnectedClients.empty()) return EMPTY_CLIENT_ID;

	auto clientId = _disconnectedClients.front();
	_disconnectedClients.pop();

	return clientId;
}

ClientId NetworkServerManager::AddClient(sf::TcpSocket* socket)
{
	std::scoped_lock lock(_mutexClients);

	for (int i = 0; i < MAX_CLIENTS; i++)
	{
		if (_clients[i].second != nullptr) continue;

		_clients[i].first = ClientId { i };
		_clients[i].second = socket;
		_udpClients[i].first = ClientId { i };

		return _clients[i].first;
	}

	return EMPTY_CLIENT_ID;
}

void NetworkServerManager::AcceptNewClients()
{
	while (Running)
	{
		auto* socket = new sf::TcpSocket();

		if (_tcpListener.accept(*socket) != sf::Socket::Done)
		{
			LOG_ERROR("Could not accept connection");
		}
		else
		{
			auto clientId = AddClient(socket);

			if (clientId.IsEmpty())
			{
				LOG_ERROR("Max clients reached (" << MAX_CLIENTS << ')');
				delete socket;
				continue;
			}

			LOG("Client connected");

			// Start a new thread to receive packets from the client
			std::thread clientReceiver = std::thread([this, clientId]() {ReceivePacketFromClient(clientId);});
			clientReceiver.detach();
		}
	}
}

void NetworkServerManager::ReceivePacketFromClient(const ClientId& clientId)
{
	sf::TcpSocket* socket;
	short counter = 0;

	for (const auto& client : _clients)
	{
		if (client.first == clientId)
		{
			socket = client.second;
			break;
		}

		counter++;
	}

	if (socket == nullptr)
	{
		LOG_ERROR("Could not find client");
		return;
	}

	while (Running)
	{
		// Receive a message from the client
		Packet* packet = PacketManager::ReceivePacket(*socket);

		if (packet->Type == static_cast<char>(PacketType::Invalid)) break;

		std::scoped_lock lock(_mutexToProcessPackets);
		_packetsToProcess.push(PacketData { packet, clientId });
	}

	{
		std::scoped_lock lock(_mutexDisconnectedClients);
		_disconnectedClients.push(clientId);
	}

	{
		std::scoped_lock lock(_mutexClients);
		delete _clients[counter].second;
		_clients[counter].second = nullptr;
		_udpClients[counter].first = EMPTY_CLIENT_ID;
	}
}

void NetworkServerManager::ReceivePacketFromUdp()
{
	while (Running)
	{
		sf::IpAddress sender;
		unsigned short port;
		sf::Packet packet;

		if (_udpSocket.receive(packet, sender, port) != sf::Socket::Done) continue;

		auto* packetData = PacketManager::FromPacket(&packet);

		if (packetData->Type == static_cast<char>(PacketType::Invalid))
		{
			delete packetData;
			continue;
		}
		else if (packetData->Type == static_cast<char>(PacketType::UDPAcknowledge))
		{
			auto* udpAcknowledgePacket = packetData->As<UDPAcknowledgePacket>();

			int index = -1;

			for (const auto& client : _clients)
			{
				index++;

				if (client.first == EMPTY_CLIENT_ID) continue;
				if (client.second->getRemoteAddress() == sender && client.second->getRemotePort() == udpAcknowledgePacket->Port)
				{
					break;
				}
			}

			if (index == MAX_CLIENTS - 1)
			{
				LOG_ERROR("Could not find client in UDP connection confirmation packet");
				delete packetData;
				continue;
			}

			std::scoped_lock lock(_mutexUdpClients);
			_udpClients[index].second = UDPClient { sender, port };
			delete packetData;

			// Send a confirmation packet to the client
			auto* confirmPacket = new ConfirmUDPConnectionPacket();
			SendPacket(confirmPacket, _udpClients[index].first, Protocol::TCP);

			continue;
		}

		ClientId clientId;

		for (const auto& client: _udpClients)
		{
			if (client.first == EMPTY_CLIENT_ID) continue;
			if (client.second.Address == sender && client.second.Port == port)
			{
				clientId = client.first;
				break;
			}
		}

		if (clientId == EMPTY_CLIENT_ID)
		{
			LOG_ERROR("Could not find client for UDP packet");
			delete packetData;
			continue;
		}

		std::scoped_lock lock(_mutexToProcessPackets);
		_packetsToProcess.push(PacketData{packetData, clientId});
	}
}
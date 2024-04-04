#include "NetworkServerManager.h"

#include "Logger.h"

#include <thread>
#include <utility>

NetworkServerManager::NetworkServerManager(unsigned short port)
{
	Running = true;

	if (_listener.listen(port) != sf::Socket::Done)
	{
		LOG_ERROR("Could not bind to port " << port);
		std::exit(EXIT_FAILURE);
	}

	LOG("Server is listening to port " << port);

	std::thread clientAcceptor = std::thread([this]() {AcceptNewClients();});
	clientAcceptor.detach();
}

NetworkServerManager::~NetworkServerManager()
{
	Running = false;
	_listener.close();
}

PacketData NetworkServerManager::PopPacket()
{
	std::scoped_lock lock(_mutexToProcessPackets);

	if (_packetsToProcess.empty()) return PacketData { nullptr, EMPTY_CLIENT_ID };

	auto packetData = _packetsToProcess.front();
	_packetsToProcess.pop();

	return packetData;
}

void NetworkServerManager::SendPacket(Packet* packet, const ClientId& clientId)
{
	auto* sfPacket = PacketManager::ToSfPacket(packet);

	{
		std::scoped_lock lock(_mutexClients);
		_clients[clientId.Index].second->send(*sfPacket);
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

		return _clients[i].first;
	}

	return EMPTY_CLIENT_ID;
}

void NetworkServerManager::AcceptNewClients()
{
	while (Running)
	{
		auto* socket = new sf::TcpSocket();

		if (_listener.accept(*socket) != sf::Socket::Done)
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
	}
}
#pragma once

#include "Packet.h"
#include "ClientId.h"
#include "Protocol.h"

struct PacketData
{
	Packet* PacketContent{};
	ClientId Client{};
};

class ServerNetworkInterface
{
public:
	/**
	 * @brief Get the next packet to process
	 * Need to delete the packet after using it
	 * @return The packet, if there is no packet it will return nullptr in the packet
	 */
	virtual PacketData PopPacket() = 0;

	/**
	 * @brief Send a packet to a specific client
	 * @param packet The packet to send
	 * @param clientIndex The index of the client to send the packet to
	 */
	virtual void SendPacket(Packet* packet, const ClientId& clientId, Protocol protocol) = 0;

	virtual ClientId PopDisconnectedClient() = 0;
};
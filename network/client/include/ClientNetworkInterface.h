#pragma once

#include "Packet.h"
#include "Protocol.h"

class ClientNetworkInterface
{
public:
	virtual ~ClientNetworkInterface() = default;

	/**
	 * @brief Get the next packet to process
	 * Need to delete the packet after using it
	 * @return
	 */
	virtual Packet* PopPacket() = 0;

	/**
	 * @brief Send a packet to the server
	 * @param packet The packet to send
	 */
	virtual void SendPacket(Packet* packet, Protocol protocol) = 0;

	/**
	 * @brief Send a UDP acknowledgment packet to the server with the client informations
	 */
	virtual void SendUDPAcknowledgmentPacket() = 0;
};
#pragma once

#include "Packet.h"

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
	virtual void SendPacket(Packet* packet) = 0;
};
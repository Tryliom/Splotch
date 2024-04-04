#include "Packet.h"

sf::Packet& operator <<(sf::Packet& packet, const Packet& myPacket)
{
	auto type = static_cast<sf::Uint8>(myPacket.Type);
	packet << type;

	myPacket.Write(packet);

	return packet;
}

sf::Packet& operator >>(sf::Packet& packet, Packet& myPacket)
{
	myPacket.Read(packet);

	return packet;
}
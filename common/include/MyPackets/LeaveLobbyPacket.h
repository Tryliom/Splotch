#pragma once

#include "MyPackets.h"

namespace MyPackets
{
	class LeaveLobbyPacket final : public Packet
	{
	public:
		LeaveLobbyPacket() : Packet(static_cast<char>(MyPacketType::LeaveLobby)) {}

		[[nodiscard]] Packet* Clone() const override { return new LeaveLobbyPacket(*this); }
		[[nodiscard]] std::string ToString() const override { return "LeaveLobbyPacket"; }
		void Write(sf::Packet& packet) const override {}
		void Read(sf::Packet& packet) override {}
	};
}
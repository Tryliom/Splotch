#pragma once

#include "MyPackets.h"

namespace MyPackets
{
	class LeaveGamePacket final : public Packet
	{
	public:
		LeaveGamePacket() : Packet(static_cast<char>(MyPacketType::LeaveGame)) {}

		[[nodiscard]] Packet* Clone() const override { return new LeaveGamePacket(*this); }
		[[nodiscard]] std::string ToString() const override { return "LeaveGamePacket"; }

		void Write(sf::Packet& packet) const override {}
		void Read(sf::Packet& packet) override {}
	};
}
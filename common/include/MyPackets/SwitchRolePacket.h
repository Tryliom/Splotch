#pragma once

#include "MyPackets.h"

namespace MyPackets
{
	class SwitchRolePacket final : public Packet
	{
	public:
		SwitchRolePacket() : Packet(static_cast<char>(MyPacketType::SwitchRole)) {}

		[[nodiscard]] Packet* Clone() const override { return new SwitchRolePacket(*this); }
		[[nodiscard]] std::string ToString() const override { return "SwitchRolePacket"; }

		void Write(sf::Packet& packet) const override {}
		void Read(sf::Packet& packet) override {}
	};
}
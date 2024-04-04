#pragma once

#include "MyPackets.h"
#include "Constants.h"

namespace MyPackets
{
	class JoinLobbyPacket final : public Packet
	{
	public:
		JoinLobbyPacket() : Packet(static_cast<char>(MyPacketType::JoinLobby)) {}

		[[nodiscard]] Packet* Clone() const override { return new JoinLobbyPacket(); }
		[[nodiscard]] std::string ToString() const override { return "JoinLobbyPacket"; }
		void Write(sf::Packet& packet) const override {}
		void Read(sf::Packet& packet) override {}
	};
}
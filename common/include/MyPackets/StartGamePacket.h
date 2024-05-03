#pragma once

#include "MyPackets.h"
#include "Constants.h"

namespace MyPackets
{
	class StartGamePacket final : public Packet
	{
	public:
		StartGamePacket() : Packet(static_cast<char>(MyPacketType::StartGame)) {}
		explicit StartGamePacket(bool isFirstNumber, bool isPlayer) : Packet(static_cast<char>(MyPacketType::StartGame)), IsFirstNumber(isFirstNumber), IsPlayer(isPlayer) {}

		bool IsFirstNumber{};
		bool IsPlayer{};

		[[nodiscard]] Packet* Clone() const override { return new StartGamePacket(*this); }
		[[nodiscard]] std::string ToString() const override { return "StartGamePacket"; }

		void Write(sf::Packet& packet) const override
		{
			packet << IsFirstNumber << IsPlayer;
		}

		void Read(sf::Packet& packet) override
		{
			packet >> IsFirstNumber >> IsPlayer;
		}
	};
}
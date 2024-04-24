#pragma once

#include "MyPackets.h"
#include "Constants.h"
#include "PlayerInputs.h"

namespace MyPackets
{
	class ConfirmInputPacket final : public Packet
	{
	public:
		ConfirmInputPacket() : Packet(static_cast<char>(MyPacketType::ConfirmationInput)) {}
		explicit ConfirmInputPacket(PlayerInput playerInput, PlayerInput handInput, int checksum)
			: Packet(static_cast<char>(MyPacketType::ConfirmationInput)), PlayerRoleInput(playerInput), GhostRoleInput(handInput), Checksum(checksum) {}

		PlayerInput PlayerRoleInput {};
		PlayerInput GhostRoleInput {};
		int Checksum {};

		[[nodiscard]] Packet* Clone() const override { return new ConfirmInputPacket(*this); }
		[[nodiscard]] std::string ToString() const override { return "ConfirmInputPacket"; }

		void Write(sf::Packet& packet) const override
		{
			packet << static_cast<sf::Uint8>(PlayerRoleInput) << static_cast<sf::Uint8>(GhostRoleInput) << Checksum;
		}

		void Read(sf::Packet& packet) override
		{
			sf::Uint8 playerInput, handInput;
			packet >> playerInput >> handInput >> Checksum;
			PlayerRoleInput = static_cast<PlayerInput>(playerInput);
			GhostRoleInput = static_cast<PlayerInput>(handInput);
		}
	};
}
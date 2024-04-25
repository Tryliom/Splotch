#pragma once

#include "MyPackets.h"
#include "Constants.h"
#include "PlayerInputs.h"
#include "Checksum.h"

namespace MyPackets
{
	class ConfirmInputPacket final : public Packet
	{
	public:
		ConfirmInputPacket() : Packet(static_cast<char>(MyPacketType::ConfirmationInput)) {}
		explicit ConfirmInputPacket(PlayerInput playerInput, PlayerInput handInput, Checksum checksum)
			: Packet(static_cast<char>(MyPacketType::ConfirmationInput)), PlayerRoleInput(playerInput), GhostRoleInput(handInput), Checksum(checksum) {}

		PlayerInput PlayerRoleInput {};
		PlayerInput GhostRoleInput {};
		Checksum Checksum {};

		[[nodiscard]] Packet* Clone() const override { return new ConfirmInputPacket(*this); }
		[[nodiscard]] std::string ToString() const override { return "ConfirmInputPacket"; }

		void Write(sf::Packet& packet) const override
		{
			packet << static_cast<sf::Uint8>(PlayerRoleInput) << static_cast<sf::Uint8>(GhostRoleInput) << Checksum.Value;
		}

		void Read(sf::Packet& packet) override
		{
			sf::Uint8 playerInput, handInput;
			packet >> playerInput >> handInput >> Checksum.Value;
			PlayerRoleInput = static_cast<PlayerInput>(playerInput);
			GhostRoleInput = static_cast<PlayerInput>(handInput);
		}
	};
}
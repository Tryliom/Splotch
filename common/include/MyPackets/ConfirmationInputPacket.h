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
		explicit ConfirmInputPacket(PlayerInput playerInput, PlayerInput handInput) : Packet(static_cast<char>(MyPacketType::ConfirmationInput)), PlayerRoleInput(playerInput), HandRoleInput(handInput) {}

		PlayerInput PlayerRoleInput {};
		PlayerInput HandRoleInput {};

		[[nodiscard]] Packet* Clone() const override { return new ConfirmInputPacket(*this); }
		[[nodiscard]] std::string ToString() const override { return "ConfirmInputPacket"; }

		void Write(sf::Packet& packet) const override
		{
			packet << static_cast<sf::Uint8>(PlayerRoleInput) << static_cast<sf::Uint8>(HandRoleInput);
		}

		void Read(sf::Packet& packet) override
		{
			sf::Uint8 playerInput, handInput;
			packet >> playerInput >> handInput;
			PlayerRoleInput = static_cast<PlayerInput>(playerInput);
			HandRoleInput = static_cast<PlayerInput>(handInput);
		}
	};
}
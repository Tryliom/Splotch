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
			: Packet(static_cast<char>(MyPacketType::ConfirmationInput)), Player1Input(playerInput), Player2Input(handInput), CurrentChecksum(checksum) {}

		PlayerInput Player1Input {};
		PlayerInput Player2Input {};
		Checksum CurrentChecksum {};

		[[nodiscard]] Packet* Clone() const override { return new ConfirmInputPacket(*this); }
		[[nodiscard]] std::string ToString() const override { return "ConfirmInputPacket"; }

		void Write(sf::Packet& packet) const override
		{
			packet << static_cast<sf::Uint8>(Player1Input) << static_cast<sf::Uint8>(Player2Input) << CurrentChecksum.Value;
		}

		void Read(sf::Packet& packet) override
		{
			sf::Uint8 playerInput, handInput;
			packet >> playerInput >> handInput >> CurrentChecksum.Value;
			Player1Input = static_cast<PlayerInput>(playerInput);
			Player2Input = static_cast<PlayerInput>(handInput);
		}
	};
}
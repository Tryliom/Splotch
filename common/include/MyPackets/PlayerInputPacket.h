#pragma once

#include <utility>

#include "MyPackets.h"
#include "Constants.h"
#include "PlayerInputs.h"

namespace MyPackets
{
	class PlayerInputPacket final : public Packet
	{
	public:
		PlayerInputPacket() : Packet(static_cast<char>(MyPacketType::PlayerInput)) {}
		explicit PlayerInputPacket(std::vector<PlayerInputPerFrame> playerInputs) : Packet(static_cast<char>(MyPacketType::PlayerInput)), LastInputs(std::move(playerInputs)) {}

		std::vector<PlayerInputPerFrame> LastInputs {};

		[[nodiscard]] Packet* Clone() const override { return new PlayerInputPacket(*this); }
		[[nodiscard]] std::string ToString() const override { return "PlayerInputPacket"; }

		void Write(sf::Packet& packet) const override
		{
			packet << static_cast<sf::Uint8>(LastInputs.size());

			for (const auto& input : LastInputs)
			{
				packet << static_cast<sf::Uint8>(input.Input);
				packet << input.Frame;
			}
		}

		void Read(sf::Packet& packet) override
		{
			sf::Uint8 size;
			packet >> size;

			LastInputs.resize(size);

			for (auto& input : LastInputs)
			{
				sf::Uint8 inputType;
				packet >> inputType;
				input.Input = static_cast<PlayerInput>(inputType);

				packet >> input.Frame;
			}
		}
	};
}
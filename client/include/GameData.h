#pragma once

#include "PlayerDrawable.h"
#include "PlayerInputs.h"

struct GameData
{
	Math::Vec2F PlayerPosition;
	HandSlot HandSlot = HandSlot::SLOT_3;
	PlayerRole PlayerRole = PlayerRole::PLAYER;

	std::array<PlayerDrawable, MAX_PLAYERS> Players;

	PlayerInput PlayerInputs{};
	PlayerInput PreviousPlayerInputs{};
	PlayerInput HandInputs{};
	PlayerInput PreviousHandInputs{};

	/**
	 * @brief Compare two GameData, players drawable and player role are not compared
	 * @param other
	 * @return
	 */
	bool operator==(const GameData& other) const
	{
		return PlayerPosition == other.PlayerPosition &&
			HandSlot == other.HandSlot &&
			PlayerInputs == other.PlayerInputs &&
			PreviousPlayerInputs == other.PreviousPlayerInputs &&
			HandInputs == other.HandInputs &&
			PreviousHandInputs == other.PreviousHandInputs;
	}
};
#pragma once

struct GameData
{
	Math::Vec2F PlayerPosition;
	HandSlot HandSlot = HandSlot::SLOT_3;
	PlayerRole PlayerRole = PlayerRole::PLAYER;
};
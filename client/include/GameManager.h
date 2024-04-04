#pragma once

#include "Constants.h"
#include "Packet.h"
#include "Vec2.h"

struct PlayerInputs
{
	bool Up = false;
	bool Down = false;
	bool Left = false;
	bool Right = false;
};

class GameManager
{
 public:
	GameManager(ScreenSizeValue width, ScreenSizeValue height);

 private:
	Math::Vec2F _playerPosition;
	HandSlot _handSlot = HandSlot::SLOT_1;

	PlayerInputs _playerInputs;
	PlayerInputs _previousPlayerInputs;
	PlayerInputs _handInputs;
	PlayerInputs _previousHandInputs;

	ScreenSizeValue _width;
	ScreenSizeValue _height;

 public:
	void OnPacketReceived(Packet& packet);

	[[nodiscard]] PlayerInputs GetPlayerInputs() const;
	[[nodiscard]] PlayerInputs GetPreviousPlayerInputs() const;
	[[nodiscard]] PlayerInputs GetHandInputs() const;
	[[nodiscard]] PlayerInputs GetPreviousHandInputs() const;

	[[nodiscard]] Math::Vec2F GetPlayerPosition() const;
};
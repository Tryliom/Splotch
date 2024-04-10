#pragma once

#include "Constants.h"
#include "Packet.h"
#include "Vec2.h"
#include "PlayerInputs.h"

#include <queue>

class GameManager
{
 public:
	GameManager(ScreenSizeValue width, ScreenSizeValue height);

 private:
	Math::Vec2F _playerPosition;
	HandSlot _handSlot = HandSlot::SLOT_1;

	PlayerInput _playerInputs;
	PlayerInput _handInputs;

	ScreenSizeValue _width;
	ScreenSizeValue _height;

	PlayerRole _playerRole = PlayerRole::PLAYER;

 public:
	void OnPacketReceived(Packet& packet);

	[[nodiscard]] PlayerInput GetPlayerInputs() const;
	[[nodiscard]] PlayerInput GetHandInputs() const;

	[[nodiscard]] Math::Vec2F GetPlayerPosition() const;
	void SetPlayerPosition(Math::Vec2F playerPosition);

	[[nodiscard]] Math::Vec2F GetHandPosition() const;
	void DecreaseHandSlot();
	void IncreaseHandSlot();

	PlayerRole GetPlayerRole();

	void SetPlayerInputs(PlayerInput playerInput);
	void SetHandInputs(PlayerInput playerInput);
};
#pragma once

#include "Constants.h"
#include "Packet.h"
#include "Vec2.h"
#include "PlayerInputs.h"

#include <queue>

struct FinalInputs
{
	PlayerInput PlayerRoleInput {};
	PlayerInput HandRoleInput {};
};

class GameManager
{
 public:
	GameManager(ScreenSizeValue width, ScreenSizeValue height);

 private:
	Math::Vec2F _playerPosition;
	HandSlot _handSlot = HandSlot::SLOT_1;

	// Player inputs from my player (hand or player role)
	std::queue<PlayerInput> _myPlayerInputs;

	// Confirmed player inputs from server (hand and player role)
	std::queue<FinalInputs> _confirmedPlayerInputs;

	ScreenSizeValue _width;
	ScreenSizeValue _height;

 public:
	void OnPacketReceived(Packet& packet);

	[[nodiscard]] PlayerInput GetPlayerInputs() const;
	[[nodiscard]] PlayerInput GetHandInputs() const;

	[[nodiscard]] Math::Vec2F GetPlayerPosition() const;

	void AddPlayerInputs(PlayerInput playerInput);
	std::vector<PlayerInputPerFrame> GetLastPlayerInputs();
};
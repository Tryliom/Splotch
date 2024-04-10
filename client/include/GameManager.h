#pragma once

#include "Constants.h"
#include "Packet.h"
#include "Vec2.h"
#include "PlayerInputs.h"

#include <queue>

enum class PlayerRole
{
	PLAYER,
	HAND
};

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

	PlayerRole _playerRole = PlayerRole::PLAYER;

 public:
	void OnPacketReceived(Packet& packet);

	[[nodiscard]] PlayerInput GetPlayerInputs() const;
	[[nodiscard]] PlayerInput GetHandInputs() const;

	[[nodiscard]] Math::Vec2F GetPlayerPosition() const;
	void SetPlayerPosition(Math::Vec2F playerPosition);

	[[nodiscard]] Math::Vec2F GetHandPosition() const;
	void SetHandSlot(HandSlot handSlot);

	PlayerRole GetPlayerRole();

	void AddPlayerInputs(PlayerInput playerInput);
	std::vector<PlayerInputPerFrame> GetLastPlayerInputs();
};
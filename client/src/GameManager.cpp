#include "GameManager.h"

#include "MyPackets.h"
#include "MyPackets/ConfirmationInputPacket.h"
#include "MyPackets/StartGamePacket.h"
#include "MyPackets/PlayerInputPacket.h"
#include "Logger.h"

GameManager::GameManager(ScreenSizeValue width, ScreenSizeValue height) : _width(width), _height(height) {}

void GameManager::OnPacketReceived(Packet& packet)
{
	if (packet.Type == static_cast<char>(MyPackets::MyPacketType::StartGame))
	{
		auto& startGamePacket = *packet.As<MyPackets::StartGamePacket>();

		_playerRole = startGamePacket.IsPlayer ? PlayerRole::PLAYER : PlayerRole::HAND;
	}
}

PlayerInput GameManager::GetPlayerInputs() const
{
	return _playerInputs;
}

PlayerInput GameManager::GetHandInputs() const
{
	return _handInputs;
}

Math::Vec2F GameManager::GetPlayerPosition() const
{
	return _playerPosition;
}

void GameManager::SetPlayerPosition(Math::Vec2F playerPosition)
{
	_playerPosition = playerPosition;
}

Math::Vec2F GameManager::GetHandPosition() const
{
	auto x = HAND_START_POSITION.X + HAND_SLOT_SIZE * static_cast<int>(_handSlot);
	return {x * _width, HAND_START_POSITION.Y * _height};
}

void GameManager::SetHandSlot(HandSlot handSlot)
{
	_handSlot = handSlot;
}

PlayerRole GameManager::GetPlayerRole()
{
	return _playerRole;
}

void GameManager::SetPlayerInputs(PlayerInput playerInput)
{
	_playerInputs = playerInput;
}

void GameManager::SetHandInputs(PlayerInput playerInput)
{
	_handInputs = playerInput;
}
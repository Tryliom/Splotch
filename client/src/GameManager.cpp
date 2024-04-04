#include "GameManager.h"

#include "MyPackets.h"
#include "MyPackets/StartGamePacket.h"

GameManager::GameManager(ScreenSizeValue width, ScreenSizeValue height) : _width(width), _height(height) {}

void GameManager::OnPacketReceived(Packet& packet)
{
	//TODO: Apply inputs from packet
}

PlayerInputs GameManager::GetPlayerInputs() const
{
	return _playerInputs;
}

PlayerInputs GameManager::GetPreviousPlayerInputs() const
{
	return _previousPlayerInputs;
}

PlayerInputs GameManager::GetHandInputs() const
{
	return _handInputs;
}

PlayerInputs GameManager::GetPreviousHandInputs() const
{
	return _previousHandInputs;
}

Math::Vec2F GameManager::GetPlayerPosition() const
{
	return _playerPosition;
}
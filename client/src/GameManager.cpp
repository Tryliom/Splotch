#include "GameManager.h"

#include "MyPackets.h"
#include "MyPackets/ConfirmationInputPacket.h"

GameManager::GameManager(ScreenSizeValue width, ScreenSizeValue height) : _width(width), _height(height) {}

void GameManager::OnPacketReceived(Packet& packet)
{
	if (packet.Type == static_cast<char>(MyPackets::MyPacketType::ConfirmationInput))
	{
		auto& confirmationInputPacket = *packet.As<MyPackets::ConfirmInputPacket>();
		_confirmedPlayerInputs.push({confirmationInputPacket.PlayerRoleInput, confirmationInputPacket.HandRoleInput});

		_myPlayerInputs.pop();
	}
}

// Temporary function to test the game
PlayerInput GameManager::GetPlayerInputs() const
{
	if (_myPlayerInputs.empty()) return {};

	return _myPlayerInputs.front();
}

PlayerInput GameManager::GetHandInputs() const
{
	if (_confirmedPlayerInputs.empty()) return {};

	return _confirmedPlayerInputs.front().HandRoleInput;
}

Math::Vec2F GameManager::GetPlayerPosition() const
{
	return _playerPosition;
}

void GameManager::AddPlayerInputs(PlayerInput playerInput)
{
	_myPlayerInputs.push(playerInput);
}

std::vector<PlayerInputPerFrame> GameManager::GetLastPlayerInputs()
{
	if (_myPlayerInputs.empty()) return {};

	// Send all last player inputs to the server
	std::vector<PlayerInputPerFrame> playerInputs;
	int currentFrame = _confirmedPlayerInputs.size();
	std::queue<PlayerInput> tempQueue = _myPlayerInputs;

	while (!tempQueue.empty())
	{
		playerInputs.push_back({ currentFrame, tempQueue.front() });
		tempQueue.pop();
	}

	return playerInputs;
}
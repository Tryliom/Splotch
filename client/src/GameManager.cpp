#include "GameManager.h"

#include "MyPackets.h"
#include "MyPackets/ConfirmationInputPacket.h"
#include "MyPackets/StartGamePacket.h"
#include "MyPackets/PlayerInputPacket.h"
#include "Logger.h"

GameManager::GameManager(ScreenSizeValue width, ScreenSizeValue height) : _width(width), _height(height) {}

void GameManager::OnPacketReceived(Packet& packet)
{
	if (packet.Type == static_cast<char>(MyPackets::MyPacketType::ConfirmationInput))
	{
		auto& confirmationInputPacket = *packet.As<MyPackets::ConfirmInputPacket>();
		_confirmedPlayerInputs.push({confirmationInputPacket.PlayerRoleInput, confirmationInputPacket.HandRoleInput});

		_myPlayerInputs.pop();
	}
	else if (packet.Type == static_cast<char>(MyPackets::MyPacketType::PlayerInput))
	{
		auto& playerInputPacket = *packet.As<MyPackets::PlayerInputPacket>();

		// Take last player input from the server, ignore if _lastOtherPlayerInput.Frame is greater than playerInputPacket.Frame
		if (playerInputPacket.LastInputs.size() > 0 && _lastOtherPlayerInput.Frame < playerInputPacket.LastInputs.front().Frame)
		{
			_lastOtherPlayerInput = playerInputPacket.LastInputs.front();
		}
	}
	else if (packet.Type == static_cast<char>(MyPackets::MyPacketType::StartGame))
	{
		auto& startGamePacket = *packet.As<MyPackets::StartGamePacket>();

		_playerRole = startGamePacket.IsPlayer ? PlayerRole::PLAYER : PlayerRole::HAND;
	}
}

// Temporary function to test the game
PlayerInput GameManager::GetPlayerInputs() const
{
	if (_playerRole == PlayerRole::PLAYER)
	{
		if (_myPlayerInputs.empty()) return {};

		return _myPlayerInputs.front();
	}
	else
	{
		if (_lastOtherPlayerInput.Frame > -1 && _lastOtherPlayerInput.Frame >= _confirmedPlayerInputs.size() - 1)
		{
			return _lastOtherPlayerInput.Input;
		}

		if (_confirmedPlayerInputs.empty()) return {};

		return _confirmedPlayerInputs.front().PlayerRoleInput;
	}
}

PlayerInput GameManager::GetHandInputs() const
{
	if (_playerRole == PlayerRole::PLAYER)
	{
		if (_lastOtherPlayerInput.Frame > -1 && _lastOtherPlayerInput.Frame > _confirmedPlayerInputs.size() - 1)
		{
			return _lastOtherPlayerInput.Input;
		}

		if (_confirmedPlayerInputs.empty()) return {};

		return _confirmedPlayerInputs.front().HandRoleInput;
	}
	else
	{
		if (_myPlayerInputs.empty()) return {};

		return _myPlayerInputs.front();
	}
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

void GameManager::AddPlayerInputs(PlayerInput playerInput)
{
	_myPlayerInputs.push(playerInput);
}

std::vector<PlayerInputPerFrame> GameManager::GetLastPlayerInputs()
{
	if (_myPlayerInputs.empty()) return {};

	// Send all last player inputs to the server
	std::vector<PlayerInputPerFrame> playerInputs = {};
	int currentFrame = _confirmedPlayerInputs.size();
	std::queue<PlayerInput> tempQueue = _myPlayerInputs;

	while (!tempQueue.empty())
	{
		playerInputs.push_back({ static_cast<int>(currentFrame + tempQueue.size()), tempQueue.front() });
		tempQueue.pop();
	}

	return playerInputs;
}
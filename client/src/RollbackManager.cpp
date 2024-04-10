#include "RollbackManager.h"

#include "MyPackets.h"
#include "MyPackets/ConfirmationInputPacket.h"
#include "MyPackets/PlayerInputPacket.h"

RollbackManager::RollbackManager()
{
	_confirmedPlayerInputs.reserve(2'000);
}

void RollbackManager::OnPacketReceived(Packet& packet)
{
	if (packet.Type == static_cast<char>(MyPackets::MyPacketType::ConfirmationInput))
	{
		auto& confirmationInputPacket = *packet.As<MyPackets::ConfirmInputPacket>();
		_confirmedPlayerInputs.push_back({confirmationInputPacket.PlayerRoleInput, confirmationInputPacket.HandRoleInput});

		_localPlayerInputs.erase(_localPlayerInputs.begin());
	}
	else if (packet.Type == static_cast<char>(MyPackets::MyPacketType::PlayerInput))
	{
		auto& playerInputPacket = *packet.As<MyPackets::PlayerInputPacket>();

		if (playerInputPacket.LastInputs.empty())
		{
			return;
		}

		const auto& lastInput = playerInputPacket.LastInputs[playerInputPacket.LastInputs.size() - 1];

		// Take last player input from the server, ignore if _lastRemotePlayerInput.Frame is greater than playerInputPacket.Frame
		if (lastInput.Frame)
		{
			_lastRemotePlayerInput = lastInput;
		}
	}
}

void RollbackManager::AddPlayerInputs(PlayerInput playerInput)
{
	_localPlayerInputs.push_back(playerInput);
}

std::vector<PlayerInputPerFrame> RollbackManager::GetLastPlayerInputs()
{
	// Send all last player inputs to the server
	std::vector<PlayerInputPerFrame> playerInputs = {};
	int currentFrame = _confirmedPlayerInputs.size();

	while (playerInputs.size() < _localPlayerInputs.size())
	{
		playerInputs.push_back({static_cast<int>(currentFrame + 1 + playerInputs.size()), _localPlayerInputs[playerInputs.size()]});
	}

	return playerInputs;
}

PlayerInput RollbackManager::GetLastPlayerInput(PlayerRole playerRole) const
{
	if (playerRole == PlayerRole::PLAYER)
	{
		if (!_localPlayerInputs.empty())
		{
			return _localPlayerInputs[_localPlayerInputs.size() - 1];
		}
	}
	else if (_lastRemotePlayerInput.Frame > _confirmedPlayerInputs.size() - 1)
	{
		return _lastRemotePlayerInput.Input;
	}

	if (!_confirmedPlayerInputs.empty())
	{
		return _confirmedPlayerInputs[_confirmedPlayerInputs.size() - 1].PlayerRoleInput;
	}

	return {};
}

PlayerInput RollbackManager::GetLastHandInput(PlayerRole playerRole) const
{
	if (playerRole == PlayerRole::HAND)
	{
		if (!_localPlayerInputs.empty())
		{
			return _localPlayerInputs[_localPlayerInputs.size() - 1];
		}
	}
	else if (_lastRemotePlayerInput.Frame > _confirmedPlayerInputs.size() - 1)
	{
		return _lastRemotePlayerInput.Input;
	}

	if (!_confirmedPlayerInputs.empty())
	{
		return _confirmedPlayerInputs[_confirmedPlayerInputs.size() - 1].HandRoleInput;
	}

	return {};
}
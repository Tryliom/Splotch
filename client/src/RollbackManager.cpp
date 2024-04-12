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

		if (!_localPlayerInputs.empty())
		{
			_localPlayerInputs.erase(_localPlayerInputs.begin());
		}

		if (!_lastRemotePlayerInputs.empty())
		{
			_lastRemotePlayerInputs.erase(_lastRemotePlayerInputs.begin());
		}

		_frameFromLastConfirmedInput = 0;
	}
	else if (packet.Type == static_cast<char>(MyPackets::MyPacketType::PlayerInput))
	{
		auto& playerInputPacket = *packet.As<MyPackets::PlayerInputPacket>();

		if (playerInputPacket.LastInputs.empty()) return;

		const auto& lastInputs = playerInputPacket.LastInputs;

		for (auto lastInput : lastInputs)
		{
			if (lastInput.Frame < _confirmedPlayerInputs.size() + _lastRemotePlayerInputs.size()) continue;

			_lastRemotePlayerInputs.push_back(lastInput);
		}
	}
}

void RollbackManager::AddPlayerInputs(PlayerInput playerInput)
{
	_frameFromLastConfirmedInput++;

	_localPlayerInputs.push_back(playerInput);
}

std::vector<PlayerInputPerFrame> RollbackManager::GetLastPlayerInputs()
{
	// Send all last player inputs to the server
	std::vector<PlayerInputPerFrame> playerInputs = {};
	int currentFrame = static_cast<int>(_confirmedPlayerInputs.size());

	while (playerInputs.size() < _localPlayerInputs.size())
	{
		playerInputs.push_back({static_cast<short>(currentFrame + playerInputs.size()), _localPlayerInputs[playerInputs.size()]});
	}

	return playerInputs;
}

PlayerInput RollbackManager::GetPlayerInput(PlayerRole playerRole, int frame) const
{
	if (frame < 0) return {};

	frame += _frameFromLastConfirmedInput;

	std::vector<PlayerInput> playerInputs = {};

	for (auto confirmedPlayerInput : _confirmedPlayerInputs)
	{
		playerInputs.push_back(confirmedPlayerInput.PlayerRoleInput);
	}

	if (playerRole == PlayerRole::PLAYER)
	{
		for (auto localPlayerInput : _localPlayerInputs)
		{
			playerInputs.push_back(localPlayerInput);
		}
	}
	else
	{
		frame -= static_cast<int>(_localPlayerInputs.size()) - static_cast<int>(_lastRemotePlayerInputs.size());

		for (auto lastRemotePlayerInput : _lastRemotePlayerInputs)
		{
			playerInputs.push_back(lastRemotePlayerInput.Input);
		}
	}

	if (playerInputs.empty()) return {};

	if (frame < playerInputs.size())
	{
		return playerInputs[frame];
	}

	return playerInputs.back();
}

PlayerInput RollbackManager::GetHandInput(PlayerRole playerRole, int frame) const
{
	if (frame < 0) return {};

	frame += _frameFromLastConfirmedInput;

	std::vector<PlayerInput> handInputs = {};

	for (auto confirmedPlayerInput : _confirmedPlayerInputs)
	{
		handInputs.push_back(confirmedPlayerInput.HandRoleInput);
	}

	if (playerRole == PlayerRole::HAND)
	{
		for (auto localPlayerInput: _localPlayerInputs)
		{
			handInputs.push_back(localPlayerInput);
		}
	}
	else
	{
		frame -= static_cast<int>(_localPlayerInputs.size()) - static_cast<int>(_lastRemotePlayerInputs.size());

		for (auto lastRemotePlayerInput : _lastRemotePlayerInputs)
		{
			handInputs.push_back(lastRemotePlayerInput.Input);
		}
	}

	if (handInputs.empty()) return {};

	if (frame < handInputs.size())
	{
		return handInputs[frame];
	}

	return handInputs.back();
}

short RollbackManager::GetCurrentFrame() const
{
	return static_cast<short>(_confirmedPlayerInputs.size() + _localPlayerInputs.size() - 1);
}
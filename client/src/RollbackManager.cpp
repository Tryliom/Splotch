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
	}
	else if (packet.Type == static_cast<char>(MyPackets::MyPacketType::PlayerInput))
	{
		auto& playerInputPacket = *packet.As<MyPackets::PlayerInputPacket>();

		if (playerInputPacket.LastInputs.empty())
		{
			return;
		}

		const auto& lastInputs = playerInputPacket.LastInputs;

		for (auto lastInput : lastInputs)
		{
			if (lastInput.Frame < _confirmedPlayerInputs.size() + _lastRemotePlayerInputs.size() - 1) continue;

			_lastRemotePlayerInputs.push_back(lastInput);
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
	int currentFrame = static_cast<int>(_confirmedPlayerInputs.size()) + 1;

	while (playerInputs.size() < _localPlayerInputs.size())
	{
		playerInputs.push_back({static_cast<int>(currentFrame + playerInputs.size()), _localPlayerInputs[playerInputs.size()]});
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
	else if (_lastRemotePlayerInputs.size() > _confirmedPlayerInputs.size())
	{
		return _lastRemotePlayerInputs[_lastRemotePlayerInputs.size() - 1].Input;
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
	else if (_lastRemotePlayerInputs.size() > _confirmedPlayerInputs.size())
	{
		return _lastRemotePlayerInputs[_lastRemotePlayerInputs.size() - 1].Input;
	}

	if (!_confirmedPlayerInputs.empty())
	{
		return _confirmedPlayerInputs[_confirmedPlayerInputs.size() - 1].HandRoleInput;
	}

	return {};
}
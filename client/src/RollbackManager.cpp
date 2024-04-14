#include "RollbackManager.h"

#include <utility>

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

			if (!_unconfirmedGameData.empty())
			{
				_confirmedGameData = _unconfirmedGameData.front();
				_unconfirmedGameData.erase(_unconfirmedGameData.begin());
				_lastConfirmedFrame++;
			}
		}
		else
		{
			// If we don't have any remote inputs, we need to check if last remote inputs are the same as the last confirmed inputs
			PlayerInput currentInput = {}, lastInput = {};

			if (_confirmedPlayerInputs.size() > 1)
			{
				const auto lastConfirmedInput = _confirmedPlayerInputs[_confirmedPlayerInputs.size() - 2];
				currentInput = _confirmedGameData.PlayerRole == PlayerRole::PLAYER ? confirmationInputPacket.HandRoleInput : confirmationInputPacket.PlayerRoleInput;
				lastInput = _confirmedGameData.PlayerRole == PlayerRole::PLAYER ? lastConfirmedInput.HandRoleInput : lastConfirmedInput.PlayerRoleInput;
			}

			if (lastInput != currentInput)
			{
				_needToRollback = true;
			}
			else if (!_unconfirmedGameData.empty())
			{
				_confirmedGameData = _unconfirmedGameData.front();
				_unconfirmedGameData.erase(_unconfirmedGameData.begin());
				_lastConfirmedFrame++;
			}
		}

		_frameFromLastConfirmedInput = 0;
	}
	else if (packet.Type == static_cast<char>(MyPackets::MyPacketType::PlayerInput))
	{
		auto& playerInputPacket = *packet.As<MyPackets::PlayerInputPacket>();

		if (playerInputPacket.LastInputs.empty() || _confirmedPlayerInputs.empty()) return;

		const auto& lastInputs = playerInputPacket.LastInputs;

		for (auto lastInput : lastInputs)
		{
			if (lastInput.Frame < _confirmedPlayerInputs.size() + _lastRemotePlayerInputs.size()) continue;

			_lastRemotePlayerInputs.push_back(lastInput);

			PlayerInput lastRemoteInput;

			if (_lastRemotePlayerInputs.size() > 1)
			{
				lastRemoteInput = _lastRemotePlayerInputs[_lastRemotePlayerInputs.size() - 2].Input;
			}
			else
			{
				lastRemoteInput = _confirmedGameData.PlayerRole == PlayerRole::PLAYER ? _confirmedPlayerInputs.back().HandRoleInput : _confirmedPlayerInputs.back().PlayerRoleInput;
			}

			if (lastRemoteInput != lastInput.Input)
			{
				_needToRollback = true;
				break;
			}
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

PlayerInput RollbackManager::GetPlayerInput(int frame) const
{
	if (frame < 0) return {};

	std::vector<PlayerInput> playerInputs = {};

	for (auto confirmedPlayerInput : _confirmedPlayerInputs)
	{
		playerInputs.push_back(confirmedPlayerInput.PlayerRoleInput);
	}

	if (_confirmedGameData.PlayerRole == PlayerRole::PLAYER)
	{
		for (auto localPlayerInput : _localPlayerInputs)
		{
			playerInputs.push_back(localPlayerInput);
		}
	}
	else
	{
		frame += _frameFromLastConfirmedInput;
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

PlayerInput RollbackManager::GetHandInput(int frame) const
{
	if (frame < 0) return {};

	std::vector<PlayerInput> handInputs = {};

	for (auto confirmedPlayerInput : _confirmedPlayerInputs)
	{
		handInputs.push_back(confirmedPlayerInput.HandRoleInput);
	}

	if (_confirmedGameData.PlayerRole == PlayerRole::HAND)
	{
		for (auto localPlayerInput: _localPlayerInputs)
		{
			handInputs.push_back(localPlayerInput);
		}
	}
	else
	{
		frame += _frameFromLastConfirmedInput;
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

void RollbackManager::SetConfirmedGameData(GameData gameData)
{
	_confirmedGameData = std::move(gameData);
	_lastConfirmedFrame++;
}

GameData RollbackManager::GetConfirmedGameData() const
{
	return _confirmedGameData;
}

short RollbackManager::GetConfirmedFrame() const
{
	return _lastConfirmedFrame;
}

short RollbackManager::GetConfirmedInputFrame() const
{
	return static_cast<short>(_confirmedPlayerInputs.size());
}

void RollbackManager::ResetUnconfirmedGameData()
{
	_unconfirmedGameData.clear();
}

void RollbackManager::AddUnconfirmedGameData(GameData gameData)
{
	_unconfirmedGameData.push_back(std::move(gameData));
}

bool RollbackManager::NeedToRollback() const
{
	return _needToRollback;
}

void RollbackManager::RollbackDone()
{
	_needToRollback = false;
}
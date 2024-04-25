#include "RollbackManager.h"

#include "MyPackets.h"
#include "MyPackets/ConfirmationInputPacket.h"
#include "MyPackets/PlayerInputPacket.h"
#include "MyPackets/StartGamePacket.h"

#include <utility>

RollbackManager::RollbackManager()
{
	_confirmedFrames.reserve(2'000);
}

void RollbackManager::OnPacketReceived(Packet& packet)
{
	if (packet.Type == static_cast<char>(MyPackets::MyPacketType::ConfirmationInput))
	{
		auto& confirmationInputPacket = *packet.As<MyPackets::ConfirmInputPacket>();

		_confirmedFrames.push_back({
			{ confirmationInputPacket.PlayerRoleInput, confirmationInputPacket.GhostRoleInput },
		  { confirmationInputPacket.Checksum }
		});

		if (!_localPlayerInputs.empty())
		{
			_localPlayerInputs.erase(_localPlayerInputs.begin());
		}

		if (!_lastRemotePlayerInputs.empty())
		{
			_lastRemotePlayerInputs.erase(_lastRemotePlayerInputs.begin());

			if (!_unconfirmedGameData.empty() && !_needToRollback)
			{
				_confirmedGameData = _unconfirmedGameData.front();
				_unconfirmedGameData.erase(_unconfirmedGameData.begin());
				_lastConfirmedFrame++;

				_integrityIsOk = _confirmedGameData.GenerateChecksum() == _confirmedFrames.back().Checksum;
			}
		}
		else
		{
			// If we don't have any remote inputs, we need to check if last remote inputs are the same as the last confirmed inputs
			PlayerInput currentInput = {}, lastInput = {};

			if (_confirmedFrames.size() > 1)
			{
				const auto lastConfirmedInput = _confirmedFrames[_confirmedFrames.size() - 2];
				currentInput = _playerRole == PlayerRole::PLAYER ? confirmationInputPacket.GhostRoleInput : confirmationInputPacket.PlayerRoleInput;
				lastInput = _playerRole == PlayerRole::PLAYER ? lastConfirmedInput.Inputs.GhostRoleInput : lastConfirmedInput.Inputs.PlayerRoleInput;
			}

			if (lastInput != currentInput)
			{
				_needToRollback = true;
			}
			else if (!_unconfirmedGameData.empty() && !_needToRollback)
			{
				_confirmedGameData = _unconfirmedGameData.front();
				_unconfirmedGameData.erase(_unconfirmedGameData.begin());
				_lastConfirmedFrame++;

				_integrityIsOk = _confirmedGameData.GenerateChecksum() == _confirmedFrames.back().Checksum;
			}
		}

		if (_lastConfirmedFrame != _confirmedFrames.size())
		{
			_needToRollback = true;
		}
	}
	else if (packet.Type == static_cast<char>(MyPackets::MyPacketType::PlayerInput))
	{
		auto& playerInputPacket = *packet.As<MyPackets::PlayerInputPacket>();

		if (playerInputPacket.LastInputs.empty() || _confirmedFrames.empty()) return;

		const auto& lastInputs = playerInputPacket.LastInputs;

		for (auto lastInput : lastInputs)
		{
			if (lastInput.Frame < _confirmedFrames.size() + _lastRemotePlayerInputs.size()) continue;

			const auto frame = lastInput.Frame;
			PlayerInput lastRemoteInput;

			if (_playerRole == PlayerRole::PLAYER)
			{
				lastRemoteInput = GetGhostInput(frame);
			}
			else
			{
				lastRemoteInput = GetPlayerInput(frame);
			}

			if (lastRemoteInput != lastInput.Input)
			{
				_needToRollback = true;
			}

			_lastRemotePlayerInputs.push_back(lastInput);
		}
	}
	else if (packet.Type == static_cast<char>(MyPackets::MyPacketType::StartGame))
	{
		auto& startGamePacket = *packet.As<MyPackets::StartGamePacket>();

		_playerRole = startGamePacket.IsPlayer ? PlayerRole::PLAYER : PlayerRole::GHOST;
	}
}

void RollbackManager::AddPlayerInputs(PlayerInput playerInput)
{
	_localPlayerInputs.push_back(playerInput);
}

std::vector<PlayerInputPerFrame> RollbackManager::GetLastLocalPlayerInputs()
{
	// Send all last player inputs to the server
	std::vector<PlayerInputPerFrame> playerInputs = {};
	int currentFrame = static_cast<int>(_confirmedFrames.size());

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

	playerInputs.reserve(_confirmedFrames.size() + _localPlayerInputs.size());

	for (auto confirmedPlayerInput : _confirmedFrames)
	{
		playerInputs.push_back(confirmedPlayerInput.Inputs.PlayerRoleInput);
	}

	if (_playerRole == PlayerRole::PLAYER)
	{
		for (auto localPlayerInput : _localPlayerInputs)
		{
			playerInputs.push_back(localPlayerInput);
		}
	}
	else
	{
		for (auto lastRemotePlayerInput : _lastRemotePlayerInputs)
		{
			playerInputs.push_back(lastRemotePlayerInput.Input);
		}

		if (playerInputs.empty()) return {};

		const std::size_t remoteInputsToReplicate = _localPlayerInputs.size() - _lastRemotePlayerInputs.size();
		const PlayerInput inputToReplicate = playerInputs.back();

		for (std::size_t i = 0; i < remoteInputsToReplicate; i++)
		{
			playerInputs.push_back(inputToReplicate);
		}
	}

	if (playerInputs.empty()) return {};

	if (frame < playerInputs.size())
	{
		return playerInputs[frame];
	}

	return playerInputs.back();
}

PlayerInput RollbackManager::GetGhostInput(int frame) const
{
	if (frame < 0) return {};

	std::vector<PlayerInput> handInputs = {};

	handInputs.reserve(_confirmedFrames.size() + _localPlayerInputs.size());

	for (auto confirmedPlayerInput : _confirmedFrames)
	{
		handInputs.push_back(confirmedPlayerInput.Inputs.GhostRoleInput);
	}

	if (_playerRole == PlayerRole::GHOST)
	{
		for (auto localPlayerInput: _localPlayerInputs)
		{
			handInputs.push_back(localPlayerInput);
		}
	}
	else
	{
		for (auto lastRemotePlayerInput : _lastRemotePlayerInputs)
		{
			handInputs.push_back(lastRemotePlayerInput.Input);
		}

		if (handInputs.empty()) return {};

		const std::size_t remoteInputsToReplicate = _localPlayerInputs.size() - _lastRemotePlayerInputs.size();
		const PlayerInput inputToReplicate = handInputs.back();

		for (std::size_t i = 0; i < remoteInputsToReplicate; i++)
		{
			handInputs.push_back(inputToReplicate);
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
	return static_cast<short>(_confirmedFrames.size() + _localPlayerInputs.size() - 1);
}

void RollbackManager::SetConfirmedGameData(ClientGameData gameData)
{
	_confirmedGameData = std::move(gameData);
	_lastConfirmedFrame++;
}

ClientGameData RollbackManager::GetConfirmedGameData() const
{
	return _confirmedGameData;
}

int RollbackManager::GetConfirmedFrame() const
{
	return _lastConfirmedFrame;
}

short RollbackManager::GetConfirmedInputFrame() const
{
	return static_cast<short>(_confirmedFrames.size());
}

void RollbackManager::ResetUnconfirmedGameData()
{
	_unconfirmedGameData.clear();
}

void RollbackManager::AddUnconfirmedGameData(ClientGameData gameData)
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
	_lastConfirmedFrame = static_cast<int>(_confirmedFrames.size());
}

bool RollbackManager::IsIntegrityOk() const
{
	return _integrityIsOk;
}

void RollbackManager::CheckIntegrity(int frame)
{
	if (frame < 0 || frame >= _confirmedFrames.size() || _confirmedFrames.size() < 2) return;

	_integrityIsOk = _confirmedGameData.GenerateChecksum() == _confirmedFrames[frame].Checksum;
}
#include "RollbackManager.h"

#include "MyPackets.h"
#include "MyPackets/ConfirmationInputPacket.h"
#include "MyPackets/PlayerInputPacket.h"
#include "MyPackets/StartGamePacket.h"
#include "Logger.h"

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
			{ confirmationInputPacket.Player1Input, confirmationInputPacket.Player2Input },
		    { confirmationInputPacket.CurrentChecksum }
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
			const auto otherPlayerNumber = _localPlayerNumber == PlayerNumber::PLAYER1 ? PlayerNumber::PLAYER2 : PlayerNumber::PLAYER1;
			PlayerInput currentInput = _localPlayerNumber == PlayerNumber::PLAYER1 ? confirmationInputPacket.Player2Input : confirmationInputPacket.Player1Input;
			PlayerInput lastInput = GetPlayerInput(otherPlayerNumber, _confirmedFrames.size() - 2);

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
		const auto otherPlayerNumber = _localPlayerNumber == PlayerNumber::PLAYER1 ? PlayerNumber::PLAYER2 : PlayerNumber::PLAYER1;

		for (auto lastInput : lastInputs)
		{
			if (lastInput.Frame < _confirmedFrames.size() + _lastRemotePlayerInputs.size()) continue;

			const auto frame = lastInput.Frame;
			PlayerInput lastRemoteInput = GetPlayerInput(otherPlayerNumber, frame);

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

		_localPlayerNumber = startGamePacket.IsFirstNumber ? PlayerNumber::PLAYER1 : PlayerNumber::PLAYER2;
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

PlayerInput RollbackManager::GetPlayerInput(PlayerNumber playerNumber, int frame) const
{
	if (frame < 0) return {};

	// If we are asking for the local player, we need to check if we are the local player 1 or 2
	const auto askForLocalPlayer = playerNumber == _localPlayerNumber;
	// Inputs asked are all the inputs that we need to ask for the frame
	std::vector<PlayerInput> inputsAsked = {};

	inputsAsked.reserve(_confirmedFrames.size() + _localPlayerInputs.size());

	for (auto confirmedPlayerInput : _confirmedFrames)
	{
		inputsAsked.push_back(playerNumber == PlayerNumber::PLAYER1 ? confirmedPlayerInput.Inputs.Player1Input : confirmedPlayerInput.Inputs.Player2Input);
	}

	if (askForLocalPlayer)
	{
		for (auto localPlayerInput : _localPlayerInputs)
		{
			inputsAsked.push_back(localPlayerInput);
		}
	}
	else
	{
		for (auto lastRemotePlayerInput : _lastRemotePlayerInputs)
		{
			inputsAsked.push_back(lastRemotePlayerInput.Input);
		}

		if (inputsAsked.empty()) return {};

		const std::size_t remoteInputsToReplicate = _localPlayerInputs.size() - _lastRemotePlayerInputs.size();
		const PlayerInput inputToReplicate = inputsAsked.back();

		for (std::size_t i = 0; i < remoteInputsToReplicate; i++)
		{
			inputsAsked.push_back(inputToReplicate);
		}
	}

	if (inputsAsked.empty()) return {};

	if (frame < inputsAsked.size())
	{
		return inputsAsked[frame];
	}

	return inputsAsked.back();
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

void RollbackManager::CheckIntegrity(int frame)
{
	if (frame < 0 || frame >= _confirmedFrames.size() || _confirmedFrames.size() < 2) return;

	_integrityIsOk = _confirmedGameData.GenerateChecksum() == _confirmedFrames[frame].Checksum;

	if (!_integrityIsOk) LOG("Integrity check failed");
}
#pragma once

#include "PlayerInputs.h"
#include "Packet.h"
#include "Constants.h"
#include "GameData.h"

#include <vector>

class RollbackManager
{
 public:
	RollbackManager();

 private:
	// PlayerDrawable inputs from my player (hand or player role) from confirm frame
	std::vector<PlayerInput> _localPlayerInputs;
	std::vector<PlayerInputPerFrame> _lastRemotePlayerInputs;

	// Confirmed player inputs from server (hand and player role)
	std::vector<FinalInputs> _confirmedPlayerInputs;

	// GameData at confirmed frame
	GameData _confirmedGameData;
	std::vector<GameData> _unconfirmedGameData;
	int _lastConfirmedFrame = -1;

	bool _needToRollback = false;

public:
	void OnPacketReceived(Packet& packet);

	void AddPlayerInputs(PlayerInput playerInput);
	std::vector<PlayerInputPerFrame> GetLastPlayerInputs();

	[[nodiscard]] PlayerInput GetPlayerInput(int frame) const;
	[[nodiscard]] PlayerInput GetHandInput(int frame) const;

	[[nodiscard]] short GetCurrentFrame() const;

	void SetConfirmedGameData(GameData gameData);
	[[nodiscard]] GameData GetConfirmedGameData() const;
	[[nodiscard]] int GetConfirmedFrame() const;
	[[nodiscard]] short GetConfirmedInputFrame() const;
	[[nodiscard]] std::size_t GetUnconfirmedGameDataSize() const;
	[[nodiscard]] short GetLocalPlayerInputsSize() const;
	[[nodiscard]] short GetRemotePlayerInputsSize() const;

	void ResetUnconfirmedGameData();
	void AddUnconfirmedGameData(GameData gameData);

	[[nodiscard]] bool NeedToRollback() const;
	void RollbackDone();
};
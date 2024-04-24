#pragma once

#include "PlayerInputs.h"
#include "Packet.h"
#include "Constants.h"
#include "ClientGameData.h"

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
	ClientGameData _confirmedGameData;
	std::vector<ClientGameData> _unconfirmedGameData;
	int _lastConfirmedFrame = -1;

	PlayerRole _playerRole = PlayerRole::PLAYER;

	bool _needToRollback = false;

	int _lastServerChecksum{};

public:
	void OnPacketReceived(Packet& packet);

	void AddPlayerInputs(PlayerInput playerInput);
	std::vector<PlayerInputPerFrame> GetLastPlayerInputs();

	[[nodiscard]] PlayerInput GetPlayerInput(int frame) const;
	[[nodiscard]] PlayerInput GetGhostInput(int frame) const;

	[[nodiscard]] short GetCurrentFrame() const;

	void SetConfirmedGameData(ClientGameData gameData);
	[[nodiscard]] ClientGameData GetConfirmedGameData() const;
	[[nodiscard]] int GetConfirmedFrame() const;
	[[nodiscard]] short GetConfirmedInputFrame() const;

	void ResetUnconfirmedGameData();
	void AddUnconfirmedGameData(ClientGameData gameData);

	[[nodiscard]] bool NeedToRollback() const;
	void RollbackDone();

	/**
	 * @brief Check the integrity of the last confirmed game data with the last server checksum
	 * @return true if the last confirmed game data is valid
	 */
	[[nodiscard]] bool CheckIntegrity() const;
};
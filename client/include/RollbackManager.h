#pragma once

#include "PlayerInputs.h"
#include "Packet.h"
#include "Constants.h"
#include "ClientGameData.h"

#include <vector>

struct ConfirmedFrame
{
	FinalInputs Inputs;
	Checksum Checksum {};
};

class RollbackManager
{
 public:
	RollbackManager();

 private:
	// PlayerDrawable inputs from my player (ghost or player role) from confirm frame
	std::vector<PlayerInput> _localPlayerInputs;
	std::vector<PlayerInputPerFrame> _lastRemotePlayerInputs;

	// Confirmed player inputs from server (ghost and player role)
	std::vector<ConfirmedFrame> _confirmedFrames;

	// GameData at confirmed frame
	ClientGameData _confirmedGameData;
	std::vector<ClientGameData> _unconfirmedGameData;
	int _lastConfirmedFrame = -1;

	PlayerNumber _localPlayerNumber = PlayerNumber::PLAYER1;
	bool _needToRollback = false;
	bool _integrityIsOk = true;

public:
	void OnPacketReceived(Packet& packet);

	void AddPlayerInputs(PlayerInput playerInput);
	std::vector<PlayerInputPerFrame> GetLastLocalPlayerInputs();

	[[nodiscard]] PlayerInput GetPlayerInput(PlayerNumber playerNumber, int frame) const;

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
	[[nodiscard]] bool IsIntegrityOk() const;
	void CheckIntegrity(int frame);
};
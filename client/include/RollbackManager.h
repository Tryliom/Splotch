#pragma once

#include "PlayerInputs.h"
#include "Packet.h"
#include "Constants.h"

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

	short _frameFromLastConfirmedInput = 0;

public:
	void OnPacketReceived(Packet& packet);

	void AddPlayerInputs(PlayerInput playerInput);
	std::vector<PlayerInputPerFrame> GetLastPlayerInputs();

	[[nodiscard]] PlayerInput GetPlayerInput(PlayerRole playerRole, int frame) const;
	[[nodiscard]] PlayerInput GetHandInput(PlayerRole playerRole, int frame) const;

	[[nodiscard]] short GetCurrentFrame() const;

};
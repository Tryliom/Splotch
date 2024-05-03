#pragma once

#include "Constants.h"
#include "Packet.h"
#include "PlayerInputs.h"
#include "ClientGameData.h"

#include <queue>

class GameManager
{
 public:
	GameManager(ScreenSizeValue width, ScreenSizeValue height);

 private:
	ClientGameData _gameData;

	ScreenSizeValue _width;
	ScreenSizeValue _height;

 public:
	void OnPacketReceived(Packet& packet);

	[[nodiscard]] PlayerRole GetLocalPlayerRole() const;

	void FixedUpdate(PlayerInput player1Input, PlayerInput player1PreviousInput, PlayerInput player2Input, PlayerInput player2PreviousInput);
	void UpdatePlayerAnimations(sf::Time elapsed, sf::Time elapsedSinceLastFixed);

	[[nodiscard]] ClientGameData GetGameData() const;
	void SetGameData(ClientGameData gameData);
};
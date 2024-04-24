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

	PlayerRole _playerRole = PlayerRole::PLAYER;

	ScreenSizeValue _width;
	ScreenSizeValue _height;

 public:
	void OnPacketReceived(Packet& packet);

	PlayerRole GetPlayerRole();

	void Update(sf::Time elapsed, PlayerInput playerInput, PlayerInput previousPlayerInput,
		PlayerInput ghostInput, PlayerInput previousGhostInput);
	void UpdatePlayerAnimations(sf::Time elapsed, sf::Time elapsedSinceLastFixed);

	[[nodiscard]] ClientGameData GetGameData() const;
	void SetGameData(ClientGameData gameData);
};
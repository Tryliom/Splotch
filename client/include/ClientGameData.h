#pragma once

#include "GameData.h"

#include "PlayerDrawable.h"
#include "Constants.h"

#include <array>

class ClientGameData final : public GameData
{
 public:
	// Only used in client, not used to compare GameData
	std::array<PlayerDrawable, MAX_PLAYERS> Players;

	void UpdatePlayersAnimations(sf::Time elapsed, sf::Time elapsedSinceLastFixed);
};
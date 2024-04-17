#include "ClientGameData.h"

void ClientGameData::UpdatePlayersAnimations(sf::Time elapsed, sf::Time elapsedSinceLastFixed)
{
	std::array<Math::Vec2F, MAX_PLAYERS> playerPositions = {
		GetNextPlayerPosition(elapsedSinceLastFixed), GetHandPosition()
	};
	const std::array<PlayerInput, MAX_PLAYERS> playerInputs = {
		_playerInputs, _handInputs
	};

	for (auto i = 0; i < MAX_PLAYERS; i++)
	{
		auto& player = Players[i];

		player.SetPosition({playerPositions[i].X, playerPositions[i].Y});
		player.Update(elapsed);

		const auto playerInput = playerInputs[i];

		// Change position harshly [Debug]
		const bool isPlayerInAir = playerPositions[i].Y < PLAYER_START_POSITION.Y * _height;
		const bool isUpPressed = IsKeyPressed(playerInput, PlayerInputTypes::Up);
		const bool isLeftPressed = IsKeyPressed(playerInput, PlayerInputTypes::Left);
		const bool isRightPressed = IsKeyPressed(playerInput, PlayerInputTypes::Right);
		const bool isIdle = !isUpPressed && !isLeftPressed && !isRightPressed;

		if (isPlayerInAir) player.SetAnimation(PlayerAnimation::JUMP);

		if (isLeftPressed)
		{
			if (!isPlayerInAir) player.SetAnimation(PlayerAnimation::WALK);
			player.SetDirection(PlayerDirection::LEFT);
		}

		if (isRightPressed)
		{
			if (!isPlayerInAir) player.SetAnimation(PlayerAnimation::WALK);
			player.SetDirection(PlayerDirection::RIGHT);
		}

		if (isIdle && !isPlayerInAir)
		{
			player.SetAnimation(PlayerAnimation::IDLE);
		}
	}
}
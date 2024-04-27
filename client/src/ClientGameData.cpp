#include "ClientGameData.h"

void ClientGameData::SetPlayersRole(bool isLocalPlayer)
{
	Players[0].SetPlayerRole(PlayerRole::PLAYER, isLocalPlayer);
	Players[1].SetPlayerRole(PlayerRole::GHOST, !isLocalPlayer);
}

void ClientGameData::UpdatePlayersAnimations(sf::Time elapsed, sf::Time elapsedSinceLastFixed)
{
	const auto& body = World.GetBody(PlayerBody);
	auto playerForces = GetNextPlayerForces();
	auto playerForce = playerForces.Force;
	auto playerVelocity = playerForces.Velocity;
	auto playerPosition = World.GetBody(PlayerBody).Position();

	playerForce += GRAVITY;

	playerVelocity += playerForce * body.InverseMass() * elapsedSinceLastFixed.asSeconds();
	playerPosition += playerVelocity * elapsedSinceLastFixed.asSeconds();

	const std::array<Math::Vec2F, MAX_PLAYERS> playerPositions = {
		playerPosition, GetGhostPosition()
	};
	const std::array<PlayerInput, MAX_PLAYERS> playerInputs = {
		_playerInputs, _ghostInputs
	};

	for (auto i = 0; i < MAX_PLAYERS; i++)
	{
		auto& player = Players[i];

		player.SetPosition({playerPositions[i].X, playerPositions[i].Y});
		player.Update(elapsed);

		const auto playerInput = playerInputs[i];

		if (i == 0) // Player role
		{
			const bool isUpPressed = IsKeyPressed(playerInput, PlayerInputTypes::Up);
			const bool isLeftPressed = IsKeyPressed(playerInput, PlayerInputTypes::Left);
			const bool isRightPressed = IsKeyPressed(playerInput, PlayerInputTypes::Right);
			const bool isIdle = !isUpPressed && !isLeftPressed && !isRightPressed;

			if (!IsPlayerOnGround) player.SetAnimation(PlayerAnimation::JUMP);

			if (isLeftPressed)
			{
				if (IsPlayerOnGround) player.SetAnimation(PlayerAnimation::WALK);
				player.SetDirection(PlayerDirection::LEFT);
			}

			if (isRightPressed)
			{
				if (IsPlayerOnGround) player.SetAnimation(PlayerAnimation::WALK);
				player.SetDirection(PlayerDirection::RIGHT);
			}

			if (isIdle && IsPlayerOnGround)
			{
				player.SetAnimation(PlayerAnimation::IDLE);
			}
		}
		else
		{
			const bool isLeftPressed = IsKeyPressed(playerInput, PlayerInputTypes::Left);
			const bool isRightPressed = IsKeyPressed(playerInput, PlayerInputTypes::Right);

			if (isLeftPressed)
			{
				player.SetDirection(PlayerDirection::LEFT);
			}

			if (isRightPressed)
			{
				player.SetDirection(PlayerDirection::RIGHT);
			}
		}
	}
}
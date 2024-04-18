#include "ClientGameData.h"

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

	std::array<Math::Vec2F, MAX_PLAYERS> playerPositions = {
		playerPosition, GetHandPosition()
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
}
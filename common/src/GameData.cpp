#include "GameData.h"

#include "Constants.h"

void GameData::StartGame(ScreenSizeValue width, ScreenSizeValue height, Physics::ContactListener* contactListener)
{
	_width = width;
	_height = height;
	PlayerPosition = {PLAYER_START_POSITION.X * _width, PLAYER_START_POSITION.Y * _height - PLAYER_SIZE_SCALED.Y / 2.f};
	Hand = HandSlot::SLOT_3;

	SetupWorld(contactListener);
}

void GameData::SetupWorld(Physics::ContactListener* contactListener)
{
	World = Physics::World();
	World.SetContactListener(contactListener);
	World.SetGravity(GRAVITY);

	// Create the base platform
	auto platformBodyRef = World.CreateBody();
	auto& platform = World.GetBody(platformBodyRef);
	auto platformColliderRef = World.CreateCollider(platformBodyRef);
	auto& platformCollider = World.GetCollider(platformColliderRef);

	platform.SetUseGravity(false);
	platform.SetBodyType(Physics::BodyType::Static);
	platform.SetPosition({PLATFORM_POSITION.X * _width, PLATFORM_POSITION.Y * _height});

	platformCollider.SetIsTrigger(false);
	platformCollider.SetRectangle({
	  { PLATFORM_MIN_BOUND.X * _width, PLATFORM_MIN_BOUND.Y * _height },
	 { PLATFORM_MAX_BOUND.X * _width, PLATFORM_MAX_BOUND.Y * _height }
	});

	// Create the player
	PlayerBody = World.CreateBody();
	auto& player = World.GetBody(PlayerBody);
	auto playerColliderRef = World.CreateCollider(PlayerBody);
	auto& playerCollider = World.GetCollider(playerColliderRef);

	player.SetUseGravity(true);
	player.SetBodyType(Physics::BodyType::Dynamic);
	player.SetPosition(PlayerPosition);

	playerCollider.SetIsTrigger(false);
	playerCollider.SetRectangle({
			  { PLAYER_MIN_BOUND.X, PLAYER_MIN_BOUND.Y },
	 { PLAYER_MAX_BOUND.X, PLAYER_MAX_BOUND.Y }
	});
}

void GameData::DecreaseHandSlot()
{
	if (Hand == HandSlot::SLOT_1) return;

	Hand = static_cast<HandSlot>(static_cast<int>(Hand) - 1);
}

void GameData::IncreaseHandSlot()
{
	if (Hand == HandSlot::SLOT_5) return;

	Hand = static_cast<HandSlot>(static_cast<int>(Hand) + 1);
}

void GameData::RegisterPlayersInputs(PlayerInput playerInput, PlayerInput previousPlayerInput, PlayerInput handInput, PlayerInput previousHandInput)
{
	_playerInputs = playerInput;
	_previousPlayerInputs = previousPlayerInput;
	_handInputs = handInput;
	_previousHandInputs = previousHandInput;
}

void GameData::Update(sf::Time elapsed)
{
	UpdatePlayer(elapsed);
	UpdateHand();

	World.Update(elapsed.asSeconds());

	PlayerPosition = World.GetBody(PlayerBody).Position();
}

void GameData::UpdatePlayer(sf::Time elapsed)
{
	auto& player = World.GetBody(PlayerBody);
	player.AddForce(GetNextPlayerForce(elapsed));
}

void GameData::UpdateHand()
{
	const bool isLeftPressed = IsKeyPressed(_handInputs, PlayerInputTypes::Left);
	const bool isRightPressed = IsKeyPressed(_handInputs, PlayerInputTypes::Right);
	const bool wasLeftPressed = IsKeyPressed(_previousHandInputs, PlayerInputTypes::Left);
	const bool wasRightPressed = IsKeyPressed(_previousHandInputs, PlayerInputTypes::Right);

	if (isLeftPressed && !wasLeftPressed)
	{
		DecreaseHandSlot();
	}

	if (isRightPressed && !wasRightPressed)
	{
		IncreaseHandSlot();
	}
}

Math::Vec2F GameData::GetNextPlayerForce(sf::Time elapsed)
{
	auto& player = World.GetBody(PlayerBody);
	auto playerPosition = player.Position(); // Copy the position
	auto playerForce = player.Force(); // Copy the force

	//TODO: Add bool to check if the player is in air, set by game manager when detect collision with platform
	const bool isPlayerInAir = playerPosition.Y < PLAYER_START_POSITION.Y * _height;
	const bool isUpPressed = IsKeyPressed(_playerInputs, PlayerInputTypes::Up);
	const bool isLeftPressed = IsKeyPressed(_playerInputs, PlayerInputTypes::Left);
	const bool isRightPressed = IsKeyPressed(_playerInputs, PlayerInputTypes::Right);
	const bool wasUpPressed = IsKeyPressed(_previousPlayerInputs, PlayerInputTypes::Up);

	if (isUpPressed && !wasUpPressed && !isPlayerInAir)
	{
		playerForce += PLAYER_JUMP;
	}

	if (isLeftPressed)
	{
		playerForce += {-PLAYER_SPEED, 0.0f};
	}

	if (isRightPressed)
	{
		playerForce += {PLAYER_SPEED, 0.0f};
	}

	return playerForce;
}

Math::Vec2F GameData::GetHandPosition() const
{
	auto x = HAND_START_POSITION.X + HAND_SLOT_SIZE * static_cast<int>(Hand);
	return {x * _width, HAND_START_POSITION.Y * _height};
}

bool GameData::operator==(const GameData& other) const
{
	return PlayerPosition == other.PlayerPosition && Hand == other.Hand;
}

int GameData::GenerateChecksum() const
{
	int checksum = 0;
	checksum += static_cast<int>(PlayerPosition.X);
	checksum += static_cast<int>(PlayerPosition.Y);
	checksum += static_cast<int>(Hand);
	return checksum;
}
#include "GameData.h"

#include "Constants.h"
#include "Logger.h"

void GameData::StartGame(ScreenSizeValue width, ScreenSizeValue height)
{
	_width = width;
	_height = height;
	PlayerPosition = {PLAYER_START_POSITION.X * _width, PLAYER_START_POSITION.Y * _height - PLAYER_SIZE_SCALED.Y / 2.f};
	Hand = HandSlot::SLOT_3;

	IsPlayerDead = false;
	IsPlayerOnGround = true;

	SetupWorld();
}

void GameData::SetupWorld()
{
	World = Physics::World();
	World.SetContactListener(this);
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
	PlayerPhysicsCollider = World.CreateCollider(PlayerBody);
	auto& playerCollider = World.GetCollider(PlayerPhysicsCollider);

	player.SetUseGravity(true);
	player.SetBodyType(Physics::BodyType::Dynamic);
	player.SetPosition(PlayerPosition);

	playerCollider.SetIsTrigger(false);
	playerCollider.SetRectangle({
			  { PLAYER_PHYSICAL_BOX_MIN_BOUND.X, PLAYER_PHYSICAL_BOX_MIN_BOUND.Y },
	 { PLAYER_PHYSICAL_BOX_MAX_BOUND.X, PLAYER_PHYSICAL_BOX_MAX_BOUND.Y }
	});

	// Create the player bottom collider
	PlayerBottomCollider = World.CreateCollider(PlayerBody);
	auto& playerBottomCollider = World.GetCollider(PlayerBottomCollider);

	playerBottomCollider.SetIsTrigger(true);
	playerBottomCollider.SetCircle({
		{ 0.f, 0.f },
		PLAYER_CIRCLE_COLLIDER_RADIUS * PLAYER_SIZE_SCALED.X
	});
	playerBottomCollider.SetOffset({0.f, PLAYER_TRIGGERS_OFFSET * PLAYER_SIZE_SCALED.Y});

	// Create the player top collider
	PlayerTopCollider = World.CreateCollider(PlayerBody);
	auto& playerTopCollider = World.GetCollider(PlayerTopCollider);

	playerTopCollider.SetIsTrigger(true);
	playerTopCollider.SetCircle({
		{ 0.f, 0.f },
		PLAYER_CIRCLE_COLLIDER_RADIUS * PLAYER_SIZE_SCALED.X
	});
	playerTopCollider.SetOffset({0.f, -PLAYER_TRIGGERS_OFFSET * PLAYER_SIZE_SCALED.Y});
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
	UpdatePlayer();
	UpdateHand();

	World.Update(elapsed.asSeconds());

	PlayerPosition = World.GetBody(PlayerBody).Position();
}

void GameData::UpdatePlayer()
{
	auto& player = World.GetBody(PlayerBody);
	auto forces = GetNextPlayerForces();
	player.AddForce(forces.Force);
	player.SetVelocity(forces.Velocity);
}

void GameData::UpdateHand()
{
	const bool isLeftPressed = IsKeyPressed(_handInputs, PlayerInputTypes::Left);
	const bool isRightPressed = IsKeyPressed(_handInputs, PlayerInputTypes::Right);
	const bool isDownPressed = IsKeyPressed(_handInputs, PlayerInputTypes::Down);
	const bool wasLeftPressed = IsKeyPressed(_previousHandInputs, PlayerInputTypes::Left);
	const bool wasRightPressed = IsKeyPressed(_previousHandInputs, PlayerInputTypes::Right);
	const bool wasDownPressed = IsKeyPressed(_previousHandInputs, PlayerInputTypes::Down);

	if (isLeftPressed && !wasLeftPressed)
	{
		DecreaseHandSlot();
	}

	if (isRightPressed && !wasRightPressed)
	{
		IncreaseHandSlot();
	}

	if (isDownPressed && !wasDownPressed)
	{
		SpawnBrick();
	}
}

void GameData::SpawnBrick()
{
	auto handPosition = GetHandPosition();
	auto brickBodyRef = World.CreateBody();
	auto& brick = World.GetBody(brickBodyRef);

	LastBrick = World.CreateCollider(brickBodyRef);

	auto& brickCollider = World.GetCollider(LastBrick);
	auto brickSize = Math::Vec2F{BRICK_SIZE.X * _width, BRICK_SIZE.Y * _height};

	handPosition.Y = BRICK_SPAWN_HEIGHT * _height;

	brick.SetUseGravity(true);
	brick.SetBodyType(Physics::BodyType::Dynamic);
	//brick.SetMass(1000.f);
	brick.SetPosition(handPosition);

	brickCollider.SetIsTrigger(false);
	brickCollider.SetRectangle({
		{ -brickSize.X / 2.f, -brickSize.Y / 2.f },
		{ brickSize.X / 2.f, brickSize.Y / 2.f }
	});
}

Forces GameData::GetNextPlayerForces()
{
	auto& player = World.GetBody(PlayerBody);
	auto playerForce = player.Force(); // Copy the force
	auto playerVelocity = Math::Vec2F::Zero();

	const bool isUpPressed = IsKeyPressed(_playerInputs, PlayerInputTypes::Up);
	const bool isLeftPressed = IsKeyPressed(_playerInputs, PlayerInputTypes::Left);
	const bool isRightPressed = IsKeyPressed(_playerInputs, PlayerInputTypes::Right);
	const bool wasUpPressed = IsKeyPressed(_previousPlayerInputs, PlayerInputTypes::Up);

	if (isUpPressed && !wasUpPressed && IsPlayerOnGround)
	{
		playerForce += PLAYER_JUMP;
	}

	if (isLeftPressed)
	{
		playerVelocity = {-PLAYER_SPEED, 0.0f};
	}
	else if (isRightPressed)
	{
		playerVelocity = {PLAYER_SPEED, 0.0f};
	}

	playerVelocity.Y = player.Velocity().Y;

	return {playerForce, playerVelocity};
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
	checksum += static_cast<int>(PlayerPosition.X * 100.f);
	checksum += static_cast<int>(PlayerPosition.Y * 100.f);
	checksum += static_cast<int>(Hand);
	checksum += IsPlayerOnGround ? 1 : 0;
	checksum += IsPlayerDead ? 1 : 0;
	checksum += PlayerBody.Generation + PlayerBody.Index;
	checksum += PlayerBottomCollider.Generation + PlayerBottomCollider.Index;
	checksum += PlayerTopCollider.Generation + PlayerTopCollider.Index;
	checksum += PlayerPhysicsCollider.Generation + PlayerPhysicsCollider.Index;
	return checksum;
}

void GameData::OnTriggerEnter(Physics::ColliderRef colliderRef, Physics::ColliderRef otherColliderRef) noexcept
{
	if (PlayerBottomCollider == colliderRef || PlayerBottomCollider == otherColliderRef)
	{
		IsPlayerOnGround = true;
	}

	if (PlayerTopCollider == colliderRef || PlayerTopCollider == otherColliderRef)
	{
		IsPlayerDead = true;
	}
}

void GameData::OnTriggerExit(Physics::ColliderRef colliderRef, Physics::ColliderRef otherColliderRef) noexcept
{
	if (PlayerBottomCollider == colliderRef || PlayerBottomCollider == otherColliderRef)
	{
		IsPlayerOnGround = false;
	}
}

void GameData::OnTriggerStay(Physics::ColliderRef colliderRef, Physics::ColliderRef otherColliderRef) noexcept
{
	if (PlayerBottomCollider == colliderRef || PlayerBottomCollider == otherColliderRef)
	{
		IsPlayerOnGround = true;
	}
}
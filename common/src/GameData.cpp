#include "GameData.h"

#include "Constants.h"

#include <bit>

void GameData::StartGame(ScreenSizeValue width, ScreenSizeValue height)
{
	_width = width;
	_height = height;
	PlayerPosition = {PLAYER_START_POSITION.X * _width, PLAYER_START_POSITION.Y * _height - PLAYER_SIZE_SCALED.Y / 2.f};
	Ghost = GhostSlot::SLOT_3;

	BrickCooldown = COOLDOWN_SPAWN_BRICK;

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
	PlatformCollider = World.CreateCollider(platformBodyRef);
	auto& platformCollider = World.GetCollider(PlatformCollider);

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

void GameData::DecreaseGhostSlot()
{
	if (Ghost == GhostSlot::SLOT_1) return;

	Ghost = static_cast<GhostSlot>(static_cast<int>(Ghost) - 1);
}

void GameData::IncreaseGhostSlot()
{
	if (Ghost == GhostSlot::SLOT_5) return;

	Ghost = static_cast<GhostSlot>(static_cast<int>(Ghost) + 1);
}

void GameData::AddPlayersInputs(PlayerInput playerInput, PlayerInput previousPlayerInput, PlayerInput ghostInput, PlayerInput previousGhostInput)
{
	_playerInputs = playerInput;
	_previousPlayerInputs = previousPlayerInput;
	_ghostInputs = ghostInput;
	_previousGhostInputs = previousGhostInput;
}

void GameData::FixedUpdate()
{
	sf::Time elapsed = sf::seconds(FIXED_TIME_STEP);

	if (BrickCooldown > 0.f)
	{
		BrickCooldown -= elapsed.asSeconds();

		if (BrickCooldown < 0.f)
		{
			BrickCooldown = 0.f;
		}
	}

	UpdatePlayer();
	UpdateGhost();

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

void GameData::UpdateGhost()
{
	const bool isLeftPressed = IsKeyPressed(_ghostInputs, PlayerInputTypes::Left);
	const bool isRightPressed = IsKeyPressed(_ghostInputs, PlayerInputTypes::Right);
	const bool isDownPressed = IsKeyPressed(_ghostInputs, PlayerInputTypes::Down);
	const bool wasLeftPressed = IsKeyPressed(_previousGhostInputs, PlayerInputTypes::Left);
	const bool wasRightPressed = IsKeyPressed(_previousGhostInputs, PlayerInputTypes::Right);
	const bool wasDownPressed = IsKeyPressed(_previousGhostInputs, PlayerInputTypes::Down);

	if (isLeftPressed && !wasLeftPressed)
	{
		DecreaseGhostSlot();
	}

	if (isRightPressed && !wasRightPressed)
	{
		IncreaseGhostSlot();
	}

	if (isDownPressed && !wasDownPressed && BrickCooldown == 0.f && !BricksPerSlot[static_cast<int>(Ghost)][MAX_BRICKS_PER_COLUMN - 1].IsAlive)
	{
		SpawnBrick();

		BrickCooldown = COOLDOWN_SPAWN_BRICK;
	}
}

void GameData::SpawnBrick()
{
	auto index = 0;

	for (int i = 0; i < MAX_BRICKS_PER_COLUMN; i++)
	{
		if (!BricksPerSlot[static_cast<int>(Ghost)][i].IsAlive)
		{
			index = i;
			break;
		}
	}

	auto handPosition = GetGhostPosition();
	auto brickBodyRef = World.CreateBody();
	auto& brick = World.GetBody(brickBodyRef);
	auto brickColliderRef = World.CreateCollider(brickBodyRef);

	BricksPerSlot[static_cast<int>(Ghost)][index] = { brickBodyRef, brickColliderRef, true};

	auto& brickCollider = World.GetCollider(BricksPerSlot[static_cast<int>(Ghost)][index].Collider);
	auto brickSize = Math::Vec2F{BRICK_SIZE.X * _width, BRICK_SIZE.Y * _height};

	handPosition.Y = BRICK_SPAWN_HEIGHT * _height;

	brick.SetUseGravity(true);
	brick.SetBodyType(Physics::BodyType::Dynamic);
	brick.SetMass(10'000'000.f); // Very high mass to avoid being moved by the player
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

Math::Vec2F GameData::GetGhostPosition() const
{
	auto x = HAND_START_POSITION.X + HAND_SLOT_SIZE * static_cast<int>(Ghost);
	return {x * _width, HAND_START_POSITION.Y * _height};
}

bool GameData::operator==(const GameData& other) const
{
	return PlayerPosition == other.PlayerPosition && Ghost == other.Ghost;
}

Checksum GameData::GenerateChecksum() const
{
	int checksum = 0;

	checksum += std::bit_cast<int>(PlayerPosition.X);
	checksum += std::bit_cast<int>(PlayerPosition.Y);
	checksum += static_cast<int>(Ghost);
	checksum += IsPlayerOnGround ? 1 : 0;
	checksum += IsPlayerDead ? 1 : 0;

	return { checksum };
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

void GameData::OnCollisionEnter(Physics::ColliderRef colliderRef, Physics::ColliderRef otherColliderRef) noexcept
{
	struct BrickData
	{
		int GhostSlotIndex;
		int BrickIndex;

		[[nodiscard]] bool IsValid() const
		{
			return GhostSlotIndex != -1 && BrickIndex != -1;
		}
	};

	static constexpr int MAX_BRICK_DATA = 2;

	// When a brick collides with platform or another brick, become static
	std::array<BrickData, MAX_BRICK_DATA> bricksData = { { {-1, -1}, {-1, -1} } };

	for (int i = 0; i < HAND_SLOT_COUNT; i++)
	{
		for (int j = 0; j < MAX_BRICKS_PER_COLUMN; j++)
		{
			if (BricksPerSlot[i][j].Collider == colliderRef)
			{
				bricksData[0] = {i, j};
			}

			if (BricksPerSlot[i][j].Collider == otherColliderRef)
			{
				bricksData[1] = {i, j};
			}
		}
	}

	if (bricksData[0].IsValid() && bricksData[1].IsValid() || colliderRef == PlatformCollider || otherColliderRef == PlatformCollider)
	{
		for (int i = 0; i < MAX_BRICK_DATA; i++)
		{
			if (bricksData[i].IsValid())
			{
				auto& brick = BricksPerSlot[bricksData[i].GhostSlotIndex][bricksData[i].BrickIndex];
				World.GetBody(brick.Body).SetBodyType(Physics::BodyType::Static);
			}
		}
	}
}
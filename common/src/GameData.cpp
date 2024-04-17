#include "GameData.h"

#include "Constants.h"

void GameData::StartGame(ScreenSizeValue width, ScreenSizeValue height)
{
	_width = width;
	_height = height;
	PlayerPosition = {PLAYER_START_POSITION.X * _width, PLAYER_START_POSITION.Y * _height};
	Hand = HandSlot::SLOT_3;
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
	PlayerPosition = GetNextPlayerPosition(elapsed);

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

Math::Vec2F GameData::GetNextPlayerPosition(sf::Time elapsed) const
{
	static constexpr float MOVE_SPEED = 200.f;
	static constexpr float FALL_SPEED = 250.f;
	static constexpr float JUMP_HEIGHT = 200.f;
	auto playerPosition = PlayerPosition;

	// Change position harshly [Debug]
	const bool isPlayerInAir = playerPosition.Y < PLAYER_START_POSITION.Y * _height;
	const bool isUpPressed = IsKeyPressed(_playerInputs, PlayerInputTypes::Up);
	const bool isLeftPressed = IsKeyPressed(_playerInputs, PlayerInputTypes::Left);
	const bool isRightPressed = IsKeyPressed(_playerInputs, PlayerInputTypes::Right);
	const bool wasUpPressed = IsKeyPressed(_previousPlayerInputs, PlayerInputTypes::Up);

	if (isUpPressed && !wasUpPressed && !isPlayerInAir)
	{
		playerPosition.Y -= JUMP_HEIGHT;
	}

	if (isLeftPressed)
	{
		playerPosition.X -= MOVE_SPEED * elapsed.asSeconds();
	}

	if (isRightPressed)
	{
		playerPosition.X += MOVE_SPEED * elapsed.asSeconds();
	}

	if (isPlayerInAir)
	{
		playerPosition.Y += FALL_SPEED * elapsed.asSeconds();
	}

	return playerPosition;
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
	return checksum;
}
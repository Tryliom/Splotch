#include "GameManager.h"

#include "MyPackets.h"
#include "MyPackets/StartGamePacket.h"

GameManager::GameManager(ScreenSizeValue width, ScreenSizeValue height) : _width(width), _height(height) {}

void GameManager::OnPacketReceived(Packet& packet)
{
	if (packet.Type == static_cast<char>(MyPackets::MyPacketType::StartGame))
	{
		auto& startGamePacket = *packet.As<MyPackets::StartGamePacket>();

		_gameData.PlayerRole = startGamePacket.IsPlayer ? PlayerRole::PLAYER : PlayerRole::HAND;
		_gameData.PlayerPosition = {PLAYER_START_POSITION.X * _width, PLAYER_START_POSITION.Y * _height};
	}
}

PlayerInput GameManager::GetPlayerInputs() const
{
	return _playerInputs;
}

PlayerInput GameManager::GetHandInputs() const
{
	return _handInputs;
}

Math::Vec2F GameManager::GetHandPosition() const
{
	auto x = HAND_START_POSITION.X + HAND_SLOT_SIZE * static_cast<int>(_gameData.HandSlot);
	return {x * _width, HAND_START_POSITION.Y * _height};
}

void GameManager::DecreaseHandSlot()
{
	if (_gameData.HandSlot == HandSlot::SLOT_1) return;

	_gameData.HandSlot = static_cast<HandSlot>(static_cast<int>(_gameData.HandSlot) - 1);
}

void GameManager::IncreaseHandSlot()
{
	if (_gameData.HandSlot == HandSlot::SLOT_5) return;

	_gameData.HandSlot = static_cast<HandSlot>(static_cast<int>(_gameData.HandSlot) + 1);
}

PlayerRole GameManager::GetPlayerRole()
{
	return _gameData.PlayerRole;
}

void GameManager::SetPlayerInputs(PlayerInput playerInput, PlayerInput previousPlayerInput)
{
	_playerInputs = playerInput;
	_previousPlayerInputs = previousPlayerInput;
}

void GameManager::SetHandInputs(PlayerInput playerInput, PlayerInput previousHandInput)
{
	_handInputs = playerInput;
	_previousHandInputs = previousHandInput;
}

void GameManager::UpdatePlayersPositions(sf::Time elapsed)
{
	_gameData.PlayerPosition = GetFuturePlayerPosition(elapsed);

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

Math::Vec2F GameManager::GetFuturePlayerPosition(sf::Time elapsed)
{
	static constexpr float MOVE_SPEED = 200.f;
	static constexpr float FALL_SPEED = 250.f;
	static constexpr float JUMP_HEIGHT = 200.f;
	auto playerPosition = _gameData.PlayerPosition;

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

GameData GameManager::GetGameData() const
{
	return _gameData;
}

void GameManager::SetGameData(GameData gameData)
{
	_gameData = gameData;
}
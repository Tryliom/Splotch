#include "GameManager.h"

#include <utility>

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

		_gameData.Players[0].SetColor(startGamePacket.IsPlayer ? sf::Color::Cyan : sf::Color::Red);
		_gameData.Players[1].SetColor(startGamePacket.IsPlayer ? sf::Color::Red : sf::Color::Cyan);
	}
}

PlayerInput GameManager::GetPlayerInputs() const
{
	return _gameData.PlayerInputs;
}

PlayerInput GameManager::GetHandInputs() const
{
	return _gameData.HandInputs;
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
	_gameData.PlayerInputs = playerInput;
	_gameData.PreviousPlayerInputs = previousPlayerInput;
}

void GameManager::SetHandInputs(PlayerInput playerInput, PlayerInput previousHandInput)
{
	_gameData.HandInputs = playerInput;
	_gameData.PreviousHandInputs = previousHandInput;
}

void GameManager::UpdatePlayersPositions(sf::Time elapsed)
{
	_gameData.PlayerPosition = GetFuturePlayerPosition(elapsed);

	const bool isLeftPressed = IsKeyPressed(_gameData.HandInputs, PlayerInputTypes::Left);
	const bool isRightPressed = IsKeyPressed(_gameData.HandInputs, PlayerInputTypes::Right);
	const bool wasLeftPressed = IsKeyPressed(_gameData.PreviousHandInputs, PlayerInputTypes::Left);
	const bool wasRightPressed = IsKeyPressed(_gameData.PreviousHandInputs, PlayerInputTypes::Right);

	if (isLeftPressed && !wasLeftPressed)
	{
		DecreaseHandSlot();
	}

	if (isRightPressed && !wasRightPressed)
	{
		IncreaseHandSlot();
	}
}

void GameManager::UpdatePlayerAnimations(sf::Time elapsed, sf::Time elapsedSinceLastFixed)
{
	std::array<Math::Vec2F, MAX_PLAYERS> playerPositions = {
		GetFuturePlayerPosition(elapsedSinceLastFixed), GetHandPosition()
	};
	const std::array<PlayerInput, MAX_PLAYERS> playerInputs = {
		_gameData.PlayerInputs, _gameData.HandInputs
	};

	for (auto i = 0; i < MAX_PLAYERS; i++)
	{
		auto& player = _gameData.Players[i];

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

Math::Vec2F GameManager::GetFuturePlayerPosition(sf::Time elapsed)
{
	static constexpr float MOVE_SPEED = 200.f;
	static constexpr float FALL_SPEED = 250.f;
	static constexpr float JUMP_HEIGHT = 200.f;
	auto playerPosition = _gameData.PlayerPosition;

	// Change position harshly [Debug]
	const bool isPlayerInAir = playerPosition.Y < PLAYER_START_POSITION.Y * _height;
	const bool isUpPressed = IsKeyPressed(_gameData.PlayerInputs, PlayerInputTypes::Up);
	const bool isLeftPressed = IsKeyPressed(_gameData.PlayerInputs, PlayerInputTypes::Left);
	const bool isRightPressed = IsKeyPressed(_gameData.PlayerInputs, PlayerInputTypes::Right);
	const bool wasUpPressed = IsKeyPressed(_gameData.PreviousPlayerInputs, PlayerInputTypes::Up);

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
	_gameData = std::move(gameData);
}
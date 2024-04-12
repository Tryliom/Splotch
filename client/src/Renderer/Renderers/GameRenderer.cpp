#include "Renderer/Renderers/GameRenderer.h"

#include "Game.h"
#include "GameManager.h"
#include "AssetManager.h"
#include "Constants.h"
#include "MyPackets/LeaveGamePacket.h"

GameRenderer::GameRenderer(Game& game, GameManager& gameManager, ScreenSizeValue width, ScreenSizeValue height) :
	_game(game), _gameManager(gameManager), _height(height), _width(width)
{
	//TODO: Tell "You are the player" / "You need to stomp the player" with a timer going to 0 given by gameManager/decreasing from it

	// Create texts
	auto title = Text(
			sf::Vector2f(_width.Value / 2.f, 20.f),
			{
					TextLine({ CustomText{ .Text = "Game", .Size = 50 }})
			}
	);

	_texts.emplace_back(title);

	if (_gameManager.GetPlayerRole() == PlayerRole::PLAYER)
	{
		_players[0].SetColor(sf::Color::Cyan);
		_players[1].SetColor(sf::Color::Red);
	}
	else
	{
		_players[0].SetColor(sf::Color::Red);
		_players[1].SetColor(sf::Color::Cyan);
	}
}

void GameRenderer::OnDraw(sf::RenderTarget& target, sf::RenderStates states) const
{
	//TODO: Draw hand and bricks

	for (auto& player : _players)
	{
		target.draw(player, states);
	}
}

void GameRenderer::OnCheckInputs(sf::Event event)
{
	if (event.type == sf::Event::KeyPressed)
	{
		if (event.key.code == sf::Keyboard::Key::Escape)
		{
			_game.SendPacket(new MyPackets::LeaveGamePacket(), Protocol::TCP);
			_game.SetState(GameState::MAIN_MENU);
		}
	}
}

void GameRenderer::OnFixedUpdate(sf::Time elapsed) {}

void GameRenderer::OnUpdate(sf::Time elapsed, sf::Time elapsedSinceLastFixed, sf::Vector2f mousePosition)
{
	std::array<Math::Vec2F, MAX_PLAYERS> playerPositions = {
		_gameManager.GetFuturePlayerPosition(elapsedSinceLastFixed),
		_gameManager.GetHandPosition()
	};
	const std::array<PlayerInput, MAX_PLAYERS> playerInputs = {
		_gameManager.GetPlayerInputs(),
		_gameManager.GetHandInputs()
	};

	for (auto i = 0; i < MAX_PLAYERS; i++)
	{
		auto& player = _players[i];

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

		if (isUpPressed && !isPlayerInAir)
		{
			player.SetAnimation(PlayerAnimation::JUMP);
		}

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

void GameRenderer::OnPacketReceived(Packet& packet)
{
	if (packet.Type == static_cast<char>(MyPackets::MyPacketType::LeaveGame))
	{
		_gameOver = true;

		_texts[1] = Text(
				sf::Vector2f(_width.Value / 2.f, 150.f),
				{
						TextLine({ CustomText{ .Text = "Other player leave", .Size = 30 }})
				}
		);
	}
}
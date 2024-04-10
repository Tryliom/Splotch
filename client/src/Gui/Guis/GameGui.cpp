#include "gui/guis/GameGui.h"

#include "Game.h"
#include "GameManager.h"
#include "AssetManager.h"
#include "Constants.h"
#include "MyPackets/LeaveGamePacket.h"

GameGui::GameGui(Game& game, GameManager& gameManager, ScreenSizeValue width, ScreenSizeValue height) :
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

	_gameManager.SetPlayerPosition({PLAYER_START_POSITION.X * _width, PLAYER_START_POSITION.Y * _height});

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

void GameGui::OnDraw(sf::RenderTarget& target, sf::RenderStates states) const
{
	//TODO: Draw hand and bricks

	for (auto& player : _players)
	{
		target.draw(player, states);
	}
}

void GameGui::OnCheckInputs(sf::Event event)
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

void GameGui::OnFixedUpdate(sf::Time elapsed) {}

void GameGui::OnUpdate(sf::Time elapsed, sf::Vector2f mousePosition)
{
	static constexpr float MOVE_SPEED = 150.f;
	static constexpr float FALL_SPEED = 300.f;
	static constexpr float JUMP_SPEED = 30.f;
	std::array<Math::Vec2F, MAX_PLAYERS> playerPositions = {
		_gameManager.GetPlayerPosition(),
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

		//TODO: If player doesn't touch anything physical, set jump animation
		//TODO: Change player body according to the player input in fixed update

		// Change position harshly [Debug]

		if (IsKeyPressed(playerInput, PlayerInputTypes::Up))
		{
			player.SetAnimation(PlayerAnimation::JUMP);

			if (i == 0)
			{
				_gameManager.SetPlayerPosition({playerPositions[i].X, playerPositions[i].Y - JUMP_SPEED});
			}
		}
		else if (IsKeyPressed(playerInput, PlayerInputTypes::Left))
		{
			player.SetAnimation(PlayerAnimation::WALK);
			player.SetDirection(PlayerDirection::LEFT);

			if (i == 0)
			{
				_gameManager.SetPlayerPosition({playerPositions[i].X - MOVE_SPEED * elapsed.asSeconds(), playerPositions[i].Y});
			}
			else
			{
				_gameManager.DecreaseHandSlot();
			}
		}
		else if (IsKeyPressed(playerInput, PlayerInputTypes::Right))
		{
			player.SetAnimation(PlayerAnimation::WALK);
			player.SetDirection(PlayerDirection::RIGHT);

			if (i == 0)
			{
				_gameManager.SetPlayerPosition({playerPositions[i].X + MOVE_SPEED * elapsed.asSeconds(), playerPositions[i].Y});
			}
			else
			{
				_gameManager.IncreaseHandSlot();
			}
		}
		else
		{
			player.SetAnimation(PlayerAnimation::IDLE);

			if (i == 0 && playerPositions[i].Y < PLAYER_START_POSITION.Y * _height)
			{
				player.SetAnimation(PlayerAnimation::JUMP);
				_gameManager.SetPlayerPosition({playerPositions[i].X, playerPositions[i].Y + FALL_SPEED * elapsed.asSeconds()});
			}
		}
	}

	playerPositions = {
		_gameManager.GetPlayerPosition(),
		_gameManager.GetHandPosition()
	};

	for (auto i = 0; i < MAX_PLAYERS; i++)
	{
		auto& player = _players[i];
		player.SetPosition({playerPositions[i].X, playerPositions[i].Y});
	}
}

void GameGui::OnPacketReceived(Packet& packet)
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
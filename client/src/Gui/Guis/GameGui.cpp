#include "gui/guis/GameGui.h"

#include "Game.h"
#include "GameManager.h"
#include "AssetManager.h"
#include "Constants.h"
#include "MyPackets/LeaveGamePacket.h"
#include "MyPackets/PlayerInputPacket.h"

GameGui::GameGui(Game& game, GameManager& gameManager, ScreenSizeValue width, ScreenSizeValue height) :
	_game(game), _gameManager(gameManager), _height(height), _width(width)
{
	//TODO: Tell "You are the player" / "You need to stomp the player" with a timer going to 0 given by gameManager/decreasing from it
	//TODO: Apply cyan color when you are the player and black when you are not
	//TODO: Apply cyan color on the hand when it's your turn and black when it's not

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

		if (event.key.code == sf::Keyboard::Key::W && !IsKeyPressed(_currentPlayerInput, PlayerInputTypes::Up))
		{
			_currentPlayerInput |= static_cast<std::uint8_t>(PlayerInputTypes::Up);
		}

		if (event.key.code == sf::Keyboard::Key::A && !IsKeyPressed(_currentPlayerInput, PlayerInputTypes::Left))
		{
			_currentPlayerInput |= static_cast<std::uint8_t>(PlayerInputTypes::Left);
		}

		if (event.key.code == sf::Keyboard::Key::D && !IsKeyPressed(_currentPlayerInput, PlayerInputTypes::Right))
		{
			_currentPlayerInput |= static_cast<std::uint8_t>(PlayerInputTypes::Right);
		}

		if (event.key.code == sf::Keyboard::Key::S && !IsKeyPressed(_currentPlayerInput, PlayerInputTypes::Down))
		{
			_currentPlayerInput |= static_cast<std::uint8_t>(PlayerInputTypes::Down);
		}
	}
}

void GameGui::OnFixedUpdate(sf::Time elapsed)
{
	_gameManager.AddPlayerInputs(_currentPlayerInput);

	// Send player input
	_game.SendPacket(new MyPackets::PlayerInputPacket(_gameManager.GetLastPlayerInputs()), Protocol::UDP);

	_currentPlayerInput = {};
}

void GameGui::OnUpdate(sf::Time elapsed, sf::Vector2f mousePosition)
{
	const std::array<Math::Vec2F, 2> playerPositions = {
		_gameManager.GetPlayerPosition(),
		_gameManager.GetHandPosition()
	};
	const std::array<PlayerInput, 2> playerInputs = {
		_gameManager.GetPlayerInputs(),
		_gameManager.GetHandInputs()
	};

	for (auto i = 0; i < 2; i++)
	{
		auto& player = _players[i];

		player.SetPosition({playerPositions[i].X, playerPositions[i].Y});
		player.Update(elapsed);

		const auto playerInput = playerInputs[i];

		//TODO: If player doesn't touch anything physical, set jump animation
		//TODO: Change player body according to the player input
		if (IsKeyPressed(playerInput, PlayerInputTypes::Up))
		{
			player.SetAnimation(PlayerAnimation::JUMP);
		}
		else if (IsKeyPressed(playerInput, PlayerInputTypes::Left))
		{
			player.SetAnimation(PlayerAnimation::WALK);
			player.SetDirection(PlayerDirection::LEFT);
		}
		else if (IsKeyPressed(playerInput, PlayerInputTypes::Right))
		{
			player.SetAnimation(PlayerAnimation::WALK);
			player.SetDirection(PlayerDirection::RIGHT);
		}
		else
		{
			player.SetAnimation(PlayerAnimation::IDLE);
		}
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
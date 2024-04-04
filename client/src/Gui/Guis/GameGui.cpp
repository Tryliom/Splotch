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
	//TODO: Apply cyan color when you are the player and black when you are not
	//TODO: Apply cyan color on the hand when it's your turn and black when it's not

	// Create texts
	auto title = Text(
			sf::Vector2f(_width.Value / 2.f, 100.f),
			{
					TextLine({ CustomText{ .Text = "Game", .Size = 50 }})
			}
	);

	_texts.emplace_back(title);

	auto leaveButton = Button(
			sf::Vector2f(_width.Value / 2.f, ScreenPercentage(0.9f) * _height),
			sf::Vector2f(200.f, 50.f),
			true
	);

	leaveButton.SetText({
			TextLine({ CustomText{ .Text = "LEAVE", .Style = sf::Text::Style::Bold, .Size = 30 }})
	});
	leaveButton.SetOnClick([this]()
	{
		_game.SendPacket(new MyPackets::LeaveGamePacket());
		_game.SetState(GameState::MAIN_MENU);
	});

	_buttons.emplace_back(leaveButton);
}

void GameGui::OnDraw(sf::RenderTarget& target, sf::RenderStates states) const
{
	//TODO: Draw hand and bricks
	target.draw(_player, states);
}

void GameGui::OnUpdate(sf::Time elapsed, sf::Vector2f mousePosition)
{
	const auto playerPosition = _gameManager.GetPlayerPosition();

	_player.Update(elapsed);
	_player.SetPosition({playerPosition.X, playerPosition.Y});

	const auto playerInput = _gameManager.GetHandInputs();
	const auto previousPlayerInput = _gameManager.GetPreviousHandInputs();

	if (!previousPlayerInput.Up && playerInput.Up)
	{
		_player.SetAnimation(PlayerAnimation::JUMP);
	}
	else if (!previousPlayerInput.Left && playerInput.Left)
	{
		_player.SetAnimation(PlayerAnimation::WALK);
		_player.SetDirection(PlayerDirection::LEFT);
	}
	else if (!previousPlayerInput.Right && playerInput.Right)
	{
		_player.SetAnimation(PlayerAnimation::WALK);
		_player.SetDirection(PlayerDirection::RIGHT);
	}
	else
	{
		_player.SetAnimation(PlayerAnimation::IDLE);
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
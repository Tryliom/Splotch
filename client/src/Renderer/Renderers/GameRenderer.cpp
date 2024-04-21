#include "Renderer/Renderers/GameRenderer.h"

#include "Game.h"
#include "GameManager.h"
#include "AssetManager.h"
#include "Constants.h"
#include "Logger.h"
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
}

void GameRenderer::OnDraw(sf::RenderTarget& target, sf::RenderStates states) const
{
	static sf::RectangleShape platform;
	platform.setSize(sf::Vector2f(PLATFORM_SIZE.X * _width, PLATFORM_SIZE.Y * _height));
	platform.setFillColor(sf::Color::White);
	platform.setOutlineColor(sf::Color::Black);
	platform.setOutlineThickness(-2.f);
	platform.setPosition(sf::Vector2f(PLATFORM_POSITION.X * _width, PLATFORM_POSITION.Y * _height));
	platform.setOrigin(platform.getSize().x / 2.f, platform.getSize().y / 2.f);

	target.draw(platform, states);

	for (auto& player : _gameManager.GetGameData().Players)
	{
		target.draw(player, states);
	}

	// Draw bricks
	for (auto handIndex = 0; handIndex < HAND_SLOT_COUNT; handIndex++)
	{
		for (const auto& brick: _gameManager.GetGameData().BricksPerSlot[handIndex])
		{
			if (!brick.IsAlive) continue;

			sf::RectangleShape brickShape;
			auto brickCollider = _gameManager.GetGameData().World.GetCollider(brick.Collider);
			auto brickPosition = brickCollider.GetPosition();
			auto brickSize = brickCollider.GetRectangle().Size();

			brickShape.setSize({brickSize.X, brickSize.Y});
			brickShape.setFillColor(sf::Color::White);
			brickShape.setOutlineColor(sf::Color::Black);
			brickShape.setOutlineThickness(-1.f);
			brickShape.setOrigin(
				brickSize.X / 2.f,
				brickSize.Y / 2.f
			);
			brickShape.setPosition(brickPosition.X, brickPosition.Y);

			target.draw(brickShape, states);
		}
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
	_gameManager.UpdatePlayerAnimations(elapsed, elapsedSinceLastFixed);
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
#include "Renderer/Renderers/GameRenderer.h"

#include "Application.h"
#include "GameManager.h"

GameRenderer::GameRenderer(Application& application, GameManager& gameManager, ScreenSizeValue width, ScreenSizeValue height) :
	_application(application), _gameManager(gameManager), _height(height), _width(width)
{
	std::array<std::string_view, LINES_COUNT> lines;

	if (_gameManager.GetLocalPlayerRole() == PlayerRole::PLAYER)
	{
		lines = { START_PLAYER_COLOR_INFO, START_PLAYER_MESSAGE_LINE_0, START_PLAYER_MESSAGE_LINE_1 };
	}
	else
	{
		lines = { START_PLAYER_COLOR_INFO, START_GHOST_MESSAGE_LINE_0, START_GHOST_MESSAGE_LINE_1 };
	}

	_texts.push_back(Text(
		sf::Vector2f(_width.Value / 2.f, _height.Value / 2.f),
		{
			TextLine({ CustomText{
				.Text = lines[0].data(),
			    .Color = sf::Color::Cyan,
			    .Size = 30,
			}}),
			TextLine({ CustomText{
				.Text = lines[1].data(),
			    .Color = sf::Color::Cyan,
			    .Size = 25,
			}}),
			TextLine({ CustomText{
				.Text = lines[2].data(),
			    .Color = sf::Color::Cyan,
			    .Size = 25,
			}})
		}
	));

	_messageTimer = MESSAGE_TIMER;
}

void GameRenderer::OnDraw(sf::RenderTarget& target, sf::RenderStates states) const
{
	ClientGameData gameData = _gameManager.GetGameData();

	static sf::RectangleShape platform;
	platform.setSize(sf::Vector2f(PLATFORM_SIZE.X * _width, PLATFORM_SIZE.Y * _height));
	platform.setFillColor(sf::Color::White);
	platform.setOutlineColor(sf::Color::Black);
	platform.setOutlineThickness(-2.f);
	platform.setPosition(sf::Vector2f(PLATFORM_POSITION.X * _width, PLATFORM_POSITION.Y * _height));
	platform.setOrigin(platform.getSize().x / 2.f, platform.getSize().y / 2.f);

	target.draw(platform, states);

	for (auto& player : gameData.Players)
	{
		target.draw(player, states);
	}

	// Draw bricks
	for (auto handIndex = 0; handIndex < HAND_SLOT_COUNT; handIndex++)
	{
		for (const auto& brick: gameData.BricksPerSlot[handIndex])
		{
			if (!brick.IsAlive) continue;

			sf::RectangleShape brickShape;
			auto brickCollider = gameData.World.GetCollider(brick.Collider);
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

	if (gameData.FreezePlayersForFrames > 0)
	{
		const auto freezeTime = gameData.FreezePlayersForFrames / (PHYSICAL_FRAME_RATE / 10);
		const auto seconds = freezeTime / 10;
		const auto milliseconds = freezeTime % 10;

		// Draw text to indicate that players are frozen for x seconds
		auto text = Text(
			sf::Vector2f(_width.Value / 2.f, _height.Value / 2.f),
			{
				TextLine({ CustomText{
					.Text = "Players are frozen for",
					.Color = sf::Color::Red,
					.Size = 30,
				}}),
				TextLine({ CustomText{
					.Text = std::to_string(seconds) + "." + std::to_string(milliseconds) + " seconds",
					.Color = sf::Color::Red,
					.Size = 30,
				}})
			}
		);

		target.draw(text, states);
	}

	if (!gameData.IsGameOver()) return;

	sf::RectangleShape rectangle(sf::Vector2f(_width.Value, _height.Value));
	rectangle.setFillColor(sf::Color(0, 0, 0, 200));
	target.draw(rectangle);
}

void GameRenderer::OnInput(sf::Event event)
{
	if (event.type != sf::Event::KeyPressed) return;
	if (event.key.code != sf::Keyboard::Key::Escape) return;

	_application.LeaveGame();
}

void GameRenderer::OnUpdate(sf::Time elapsed, sf::Time elapsedSinceLastFixed, sf::Vector2f mousePosition)
{
	_gameManager.UpdatePlayerAnimations(elapsed, elapsedSinceLastFixed);

	if (_messageTimer == 0.f) return;

	_messageTimer -= elapsed.asSeconds();

	_texts[0].SetColor(LerpColor(START_COLOR, END_COLOR, 1.f - _messageTimer / MESSAGE_TIMER));

	if (_messageTimer <= 0.f)
	{
		_messageTimer = 0.f;
		_texts[0] = Text();
	}
}

void GameRenderer::OnEvent(Event event)
{
	if (event == Event::PLAYER_LEAVE_GAME)
	{
		_texts[0] = Text(
			sf::Vector2f(_width.Value / 2.f, 150.f),
			{
				TextLine({CustomText{.Text = "Other player leave", .Size = 30}})
			}
		);
	}
	else if (event == Event::WIN_GAME)
	{
		_texts[0] = Text(
			sf::Vector2f(_width.Value / 2.f, 150.f),
			{
				TextLine({CustomText{.Text = WIN_MESSAGE.data(), .Size = 30}})
			}
		);
	}
	else if (event == Event::LOSE_GAME)
	{
		_texts[0] = Text(
			sf::Vector2f(_width.Value / 2.f, 150.f),
			{
				TextLine({CustomText{.Text = LOSE_MESSAGE.data(), .Size = 30}})
			}
		);
	}

	auto leaveButton = Button(
		sf::Vector2f(_width.Value / 2.f, ScreenPercentage(0.9f) * _height),
		sf::Vector2f(200, 50),
		true
	);

	leaveButton.SetText({
        TextLine({ CustomText{ .Text = "LEAVE", .Style = sf::Text::Style::Bold, .Size = 24 }})
    });
	leaveButton.SetOnClick([this](){ _application.LeaveGame(); });

	_buttons.emplace_back(leaveButton);
}

sf::Color GameRenderer::LerpColor(sf::Color a, sf::Color b, float t)
{
	return {
		static_cast<sf::Uint8>(a.r + t * (b.r - a.r)),
		static_cast<sf::Uint8>(a.g + t * (b.g - a.g)),
		static_cast<sf::Uint8>(a.b + t * (b.b - a.b)),
		static_cast<sf::Uint8>(a.a + t * (b.a - a.a))
	};
}
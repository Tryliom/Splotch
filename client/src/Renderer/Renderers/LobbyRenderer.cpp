#include "Renderer/Renderers/LobbyRenderer.h"

#include "Application.h"

LobbyRenderer::LobbyRenderer(Application& game, ScreenSizeValue width, ScreenSizeValue height) :
		_game(game),
		_height(height),
		_width(width)
{
	auto leaveButton = Button(
			sf::Vector2f(_width.Value / 2.f, ScreenPercentage(0.9f) * _height),
			sf::Vector2f(200, 50),
			true
	);

	leaveButton.SetText({
			TextLine({ CustomText{ .Text = "LEAVE", .Style = sf::Text::Style::Bold, .Size = 24 }})
	});
	leaveButton.SetOnClick([this](){ _game.LeaveLobby(); });

	_buttons.emplace_back(leaveButton);

	// Create texts
	auto title = Text(
			sf::Vector2f(_width.Value / 2.f, ScreenPercentage { 0.1f} * width),
			{ TextLine({ CustomText{ .Text = "Lobby", .Size = 50 }}) }
	);
	auto waiting = Text(
			sf::Vector2f(_width.Value / 2.f, _height.Value / 2.f),
			{ TextLine({ CustomText{ .Text = "Waiting for opponent...", .Size = 30 }}) }
	);

	_texts.emplace_back(title);
	_texts.emplace_back(waiting);
}

void LobbyRenderer::OnEvent(Event event)
{
	if (event != Event::START_GAME) return;

	_game.StartGame();
}

#include "Renderer/Renderers/MenuRenderer.h"

#include "Application.h"

MenuRenderer::MenuRenderer(Application& game, ScreenSizeValue width, ScreenSizeValue height) : _application(game)
{
	// Create buttons
	auto playButton = Button(
		sf::Vector2f(width.Value / 2.f, height.Value * 3.f / 4.f),
		sf::Vector2f(200, 50),
		true
	);

	playButton.SetText({
		TextLine({CustomText{ .Text = "CONNECTING..", .Style = sf::Text::Style::Bold, .Size = 24}})
	});

	_buttons.emplace_back(playButton);

	auto quitButton = Button(
		sf::Vector2f(width.Value / 2.f, height.Value - 100.f),
		sf::Vector2f(200, 50),
		true
	);

	quitButton.SetText({
		TextLine({CustomText{.Text = "QUIT", .Style = sf::Text::Style::Bold, .Size = 24}})
	});
	quitButton.SetOnClick([&game]() {
		game.Quit();
	});

	_buttons.emplace_back(quitButton);

	// Create texts
	auto title = Text(
		sf::Vector2f(width.Value / 2.f, ScreenPercentage { 0.1f} * width),
		{
			TextLine({CustomText{.Text = "Splotch", .Size = 50}}),
			TextLine({CustomText{.Text = "By Tryliom", .Style = sf::Text::Style::Italic, .Size = 16}})
		}
	);

	_texts.emplace_back(title);
}

void MenuRenderer::OnEvent(Event event)
{
	if (event != Event::READY_TO_PLAY) return;

	_buttons[0].SetText({
		TextLine({CustomText{.Text = "PLAY", .Style = sf::Text::Style::Bold, .Size = 24}})
	});
	_buttons[0].SetOnClick([this]() {
	  _application.JoinLobby();
	});
}
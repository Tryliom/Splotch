#include "gui/guis/MenuGui.h"

#include "Game.h"
#include "AssetManager.h"

MenuGui::MenuGui(Game& game, ScreenSizeValue width, ScreenSizeValue height) : _game(game)
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

void MenuGui::OnUpdate(sf::Time elapsed, sf::Vector2f mousePosition)
{
	if (_game.IsReadyToPlay())
	{
		_buttons[0].SetText({
			TextLine({CustomText{.Text = "PLAY", .Style = sf::Text::Style::Bold, .Size = 24}})
		});
		_buttons[0].SetOnClick([this]() {
			_game.SetState(GameState::GAME);
		});
	}
	else
	{
		_buttons[0].SetText({
			TextLine({CustomText{.Text = "CONNECTING..", .Style = sf::Text::Style::Bold, .Size = 24}})
		});
	}
}

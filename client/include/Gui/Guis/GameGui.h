#pragma once

#include "gui/Gui.h"
#include "Constants.h"
#include "Player.h"
#include "PlayerInputs.h"

class Game;
class GameManager;

class GameGui final :
	public Gui
{
 private:
	Game& _game;
	GameManager& _gameManager;

	std::array<Player, 2> _players;
	PlayerInput _currentPlayerInput{};

	ScreenSizeValue _height;
	ScreenSizeValue _width;

	bool _gameOver = false;

	void OnDraw(sf::RenderTarget& target, sf::RenderStates states) const override;
	void OnCheckInputs(sf::Event event) override;

 public:
	explicit GameGui(Game& game, GameManager& gameManager, ScreenSizeValue width, ScreenSizeValue height);

	void OnFixedUpdate(sf::Time elapsed) override;
	void OnUpdate(sf::Time elapsed, sf::Vector2f mousePosition) override;
	void OnPacketReceived(Packet& packet) override;
};
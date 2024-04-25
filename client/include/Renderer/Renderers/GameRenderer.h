#pragma once

#include "Renderer/Renderer.h"
#include "Constants.h"

class Application;
class GameManager;

class GameRenderer final :
	public Renderer
{
 private:
	Application& _application;
	GameManager& _gameManager;

	ScreenSizeValue _height;
	ScreenSizeValue _width;

	bool _gameOver = false;

	void OnDraw(sf::RenderTarget& target, sf::RenderStates states) const override;
	void OnInput(sf::Event event) override;

 public:
	explicit GameRenderer(Application& game, GameManager& gameManager, ScreenSizeValue width, ScreenSizeValue height);

	void OnFixedUpdate(sf::Time elapsed) override;
	void OnUpdate(sf::Time elapsed, sf::Time elapsedSinceLastFixed, sf::Vector2f mousePosition) override;
	void OnEvent(Event event) override;
};
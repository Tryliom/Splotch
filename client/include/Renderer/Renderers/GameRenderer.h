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

	static constexpr short LINES_COUNT = 2;
	static constexpr std::string_view START_PLAYER_MESSAGE_LINE_0 = "Evade and climb on bricks !";
	static constexpr std::string_view START_PLAYER_MESSAGE_LINE_1 = "Use WASD to move";
	static constexpr std::string_view START_GHOST_MESSAGE_LINE_0 = "Crush the player under bricks !";
	static constexpr std::string_view START_GHOST_MESSAGE_LINE_1 = "Use AD to move and S to spawn bricks";
	const sf::Color START_COLOR = sf::Color::Cyan;
	const sf::Color END_COLOR = sf::Color::White;

	static constexpr float MESSAGE_TIMER = 6.f;
	float _messageTimer = 0;

	bool _gameOver = false;

	void OnDraw(sf::RenderTarget& target, sf::RenderStates states) const override;
	void OnInput(sf::Event event) override;

	static sf::Color LerpColor(sf::Color a, sf::Color b, float t) ;

 public:
	explicit GameRenderer(Application& game, GameManager& gameManager, ScreenSizeValue width, ScreenSizeValue height);

	void OnFixedUpdate(sf::Time elapsed) override;
	void OnUpdate(sf::Time elapsed, sf::Time elapsedSinceLastFixed, sf::Vector2f mousePosition) override;
	void OnEvent(Event event) override;
};
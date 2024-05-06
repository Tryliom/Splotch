#pragma once

#include "Renderer/Renderer.h"
#include "Constants.h"

class Application;
class GameManager;

/**
 * @brief Renderer for the game screen used in Application
 */
class GameRenderer final :
	public Renderer
{
 private:
	/**
	 * @brief Reference to the application, used to leave game
	 */
	Application& _application;
	/**
	 * @brief Reference to the game manager, used to get game state and draw it
	 */
	GameManager& _gameManager;

	/**
	 * @brief Height of the screen in pixels
	 */
	ScreenSizeValue _height;
	/**
	 * @brief Width of the screen in pixels
	 */
	ScreenSizeValue _width;

	/**
	 * @brief Texts to display on the screen at the start of the game from the player's perspective
	 */
	static constexpr short LINES_COUNT = 3;
	static constexpr std::array<std::string_view, LINES_COUNT> START_PLAYER_MESSAGE = {
		"You're blue !",
		"Evade and climb on bricks !",
		"Use WASD to move"
	};
	static constexpr std::array<std::string_view, LINES_COUNT> START_GHOST_MESSAGE = {
		"You're blue !",
		"Crush the player under bricks !",
		"Use AD to move and S to spawn bricks"
	};
	const sf::Color MESSAGE_START_COLOR = sf::Color::Cyan;
	const sf::Color MESSAGE_END_COLOR = sf::Color::White;
	static constexpr float MESSAGE_TIMER = 6.f;
	float _messageTimer = 0;

	/**
	 * @brief Texts to display on the screen at the end of the game
	 */
	static constexpr std::string_view WIN_MESSAGE = "You win !";
	static constexpr std::string_view LOSE_MESSAGE = "You lose !";

	void OnDraw(sf::RenderTarget& target, sf::RenderStates states) const override;
	void OnInput(sf::Event event) override;

	/**
	 * @brief Linear interpolation between two colors
	 *
	 * @param a First color
	 * @param b Second color
	 * @param t Interpolation value
	 * @return Interpolated color
	 */
	static sf::Color LerpColor(sf::Color a, sf::Color b, float t);

 public:
	/**
	 * @brief Construct a new GameRenderer object <br>
	 * It initializes the texts to display at the start of the game depending on the player's role
	 *
	 * @param application Reference to the application
	 * @param gameManager Reference to the game manager
	 * @param width Width of the screen in pixels
	 * @param height Height of the screen in pixels
	 */
	explicit GameRenderer(Application& application, GameManager& gameManager, ScreenSizeValue width, ScreenSizeValue height);

	/**
	 * @brief Update the game renderer <br>
	 * It updates the message timer, texts and players animations
	 *
	 * @param elapsed Time elapsed since the last update
	 * @param elapsedSinceLastFixed Time elapsed since the last fixed update
	 * @param mousePosition Position of the mouse on the screen
	 */
	void OnUpdate(sf::Time elapsed, sf::Time elapsedSinceLastFixed, sf::Vector2f mousePosition) override;
	void OnEvent(Event event) override;
};
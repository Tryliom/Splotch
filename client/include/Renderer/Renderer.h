#pragma once

#include "Button.h"
#include "Text.h"
#include "Event.h"

#include <SFML/Graphics.hpp>

/**
 * @brief Base class for all renderers in the application <br>
 * It can be used to add buttons and texts to the screen, draw them and handle input
 */
class Renderer : public sf::Drawable
{
public:
	Renderer() = default;

protected:
	/**
	 * @brief Buttons to display on the screen
	 */
	std::vector<Button> _buttons;
	/**
	 * @brief Texts to display on the screen
	 */
	std::vector<Text> _texts;

	/**
	 * @brief Draw the renderer on the screen and all its elements (buttons and texts), it calls OnDraw to draw additional elements from the derived class
	 * @param target Render target to draw on
	 * @param states Render states to use
	 */
	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

	/**
	 * @brief Handle update, called every frame
	 * @param elapsed Time since last frame
	 * @param elapsedSinceLastFixed Time elapsed since the last fixed update
	 * @param mousePosition Position of the mouse on the screen
	 */
	virtual void OnUpdate(sf::Time elapsed, sf::Time elapsedSinceLastFixed, sf::Vector2f mousePosition) {}

	/**
	 * @brief Draw additional elements after background and before buttons and texts
	 */
	virtual void OnDraw(sf::RenderTarget& target, sf::RenderStates states) const {}
	/**
	 * @brief Handle input events
	 * @param event Input event
	 */
	virtual void OnInput(sf::Event event) {}

public:
	/**
	 * @brief Update the renderer and all its elements
	 * @param elapsed Time since last frame
	 * @param elapsedSinceLastFixed Time elapsed since the last fixed update
	 * @param mousePosition Position of the mouse on the screen
	 */
	void Update(sf::Time elapsed, sf::Time elapsedSinceLastFixed, sf::Vector2f mousePosition);
	/**
	 * @brief Handle input events
	 * @param event Input event
	 */
	void Input(sf::Event event);

	/**
	 * @brief Handle events coming from the application
	 * @param event Event to handle
	 */
	virtual void OnEvent(Event event) {}
};

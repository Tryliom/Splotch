#pragma once

#include "Button.h"
#include "Text.h"
#include "Event.h"

#include <SFML/Graphics.hpp>

class Renderer : public sf::Drawable
{
public:
	Renderer() = default;

protected:
	std::vector<Button> _buttons;
	std::vector<Text> _texts;

	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

	virtual void OnFixedUpdate(sf::Time elapsed) {}
	virtual void OnUpdate(sf::Time elapsed, sf::Time elapsedSinceLastFixed, sf::Vector2f mousePosition) {}
	/**
	 * @brief Draw additional elements after background and before buttons and texts
	 */
	virtual void OnDraw(sf::RenderTarget& target, sf::RenderStates states) const {}
	virtual void OnInput(sf::Event event) {}

public:
	void FixedUpdate(sf::Time elapsed);
	void Update(sf::Time elapsed, sf::Time elapsedSinceLastFixed, sf::Vector2f mousePosition);
	void Input(sf::Event event);

	virtual void OnEvent(Event event) {}
};

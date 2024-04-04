#pragma once

#include "Button.h"
#include "Text.h"
#include "Packet.h"

#include <SFML/Graphics.hpp>

class Gui : public sf::Drawable
{
public:
	Gui() = default;

protected:
	std::vector<Button> _buttons;
	std::vector<Text> _texts;

	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

	virtual void OnUpdate(sf::Time elapsed, sf::Vector2f mousePosition) {}
	/**
	 * @brief Draw additional elements after background and before buttons and texts
	 */
	virtual void OnDraw(sf::RenderTarget& target, sf::RenderStates states) const {}
	virtual void OnCheckInputs(sf::Event event) {}

public:
	void Update(sf::Time elapsed, sf::Vector2f mousePosition);
	void CheckInputs(sf::Event event);

	virtual void OnPacketReceived(Packet& packet) {}
};

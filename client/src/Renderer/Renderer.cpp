#include "Renderer/Renderer.h"

void Renderer::draw(sf::RenderTarget& target, const sf::RenderStates states) const
{
	// Call the derived class's draw method
	OnDraw(target, states);

	// Draw buttons and texts
	for (auto& button : _buttons)
	{
		if (!button.IsDisabled())
		{
			target.draw(button, states);
		}
	}

	for (auto& text : _texts)
	{
		target.draw(text, states);
	}
}

void Renderer::Update(const sf::Time elapsed, sf::Time elapsedSinceLastFixed, sf::Vector2f mousePosition)
{
	// Update buttons by checking if the mouse is hovering over them
	for (auto& button : _buttons)
	{
		if (button.IsDisabled())
		{
			continue;
		}

		button.Update(elapsed);

		if (button.GetGlobalBounds().contains(mousePosition))
		{
			if (!button.IsHover())
			{
				button.OnStartHover();
			}
		}
		else
		{
			if (button.IsHover())
			{
				button.OnEndHover();
			}
		}
	}

	// Call the derived class's update method
	OnUpdate(elapsed, elapsedSinceLastFixed, mousePosition);
}

void Renderer::Input(sf::Event event)
{
	// Call the derived class's input method
	OnInput(event);

	// Handle button clicks
	if (event.type == sf::Event::MouseButtonPressed)
	{
		if (event.mouseButton.button == sf::Mouse::Left)
		{
			for (auto& button : _buttons)
			{
				if (button.IsHover() && !button.IsDisabled())
				{
					button.OnClick();
					break;
				}
			}
		}
	}

}

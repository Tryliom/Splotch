#include "Renderer/Renderer.h"

void Renderer::draw(sf::RenderTarget& target, const sf::RenderStates states) const
{
	OnDraw(target, states);

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

void Renderer::FixedUpdate(const sf::Time elapsed)
{
	OnFixedUpdate(elapsed);
}

void Renderer::Update(const sf::Time elapsed, sf::Time elapsedSinceLastFixed, sf::Vector2f mousePosition)
{
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

	OnUpdate(elapsed, elapsedSinceLastFixed, mousePosition);
}

void Renderer::Input(sf::Event event)
{
	OnInput(event);

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

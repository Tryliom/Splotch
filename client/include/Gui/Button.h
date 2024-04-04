#pragma once

#include "Text.h"

#include <SFML/Graphics.hpp>

#include <functional>

const sf::Time HOVER_TIME = sf::seconds(0.15f);

struct TempColor
{
	int R;
	int G;
	int B;
	int A;

	explicit TempColor(sf::Color color);

	TempColor(int r, int g, int b, int a);

	explicit operator sf::Color() const;

	TempColor operator+(const TempColor& other) const;

	TempColor operator-(const TempColor& other) const;

	TempColor operator*(float factor) const;
};

class Button : public sf::Drawable
{
public:
	Button(sf::Vector2f position, sf::Vector2f size, bool centered = false);

protected:
	sf::Vector2f _position;
	sf::RectangleShape _background;
	Text _text;

	sf::Color _backgroundColor;
	sf::Color _hoverBackgroundColor;
	sf::Color _borderColor;
	sf::Color _hoverBorderColor;
	sf::Color _selectedBorderColor;
	float _borderThickness{ -1.0f };
	float _hoverBorderThickness{ -1.0f };

	bool _hover{ false };
	bool _centered{ false };
	bool _disabled{ false };
	bool _selected{ false };

	sf::Time _hoverTime{ sf::Time::Zero };

	std::function<void()> _onClick;

	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

public:
	virtual void Update(sf::Time elapsed);

	virtual void OnClick();
	virtual void OnStartHover();
	virtual void OnEndHover();

	void SetText(const std::vector<TextLine>& texts);

	sf::FloatRect GetGlobalBounds() const;

	bool IsHover() const;

	void SetOnClick(const std::function<void()>& onClick);

	void Enable()
	{
		_disabled = false;
	}

	void Disable()
	{
		_disabled = true;
	}

	bool IsDisabled() const
	{
		return _disabled;
	}

	void Toggle(const bool toggle)
	{
		_selected = toggle;
	}

	void Toggle()
	{
		_selected = !_selected;
	}
};

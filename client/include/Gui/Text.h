#pragma once

#include <SFML/Graphics.hpp>

struct CustomText
{
	std::string Text;
	sf::Color Color{ sf::Color::Cyan };
	sf::Text::Style Style{ sf::Text::Style::Regular };
	int Size{ 12 };

	[[nodiscard]] sf::Text ToSfText() const;
};

struct TextLine
{
	std::vector<CustomText> Texts;
};

class Text : public sf::Drawable
{
public:
	Text() = default;
	Text(sf::Vector2f position, const std::vector<TextLine>& texts, float maxX = -1, bool centered = true);

protected:
	std::vector<sf::Text> _texts;
	bool _centered{ false };

	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

public:
	virtual void Update(sf::Time elapsed) {}

	void SetColor(const sf::Color& color);
};
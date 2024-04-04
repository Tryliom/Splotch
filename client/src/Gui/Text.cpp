#include "gui/Text.h"

#include "AssetManager.h"

sf::Text CustomText::ToSfText() const
{
	sf::Text text;
	text.setString(Text);
	text.setFillColor(Color);
	text.setStyle(Style);
	text.setCharacterSize(Size);

	if (!AssetManager::IsInitialized()) return text;

	text.setFont(AssetManager::GetMainFont());

	return text;
}

Text::Text(sf::Vector2f position, const std::vector<TextLine>& texts, float maxX, const bool centered)
{
	_centered = centered;

	for (auto& line : texts)
	{
		sf::Text text;
		float maxHeight = 0.f;
		const float baseX = position.x;

		for (auto& customText : line.Texts)
		{
			text = customText.ToSfText();
			text.setPosition(position);

			if (_centered)
			{
				const auto bounds = text.getGlobalBounds();
				text.setOrigin(bounds.width / 2, bounds.height * 0.75f);
			}

			if (maxX > 0.f)
			{
				const auto bounds = text.getLocalBounds();

				if (position.x - baseX + bounds.width > maxX)
				{
					position.x = baseX;
					position.y += maxHeight * 1.5f;
					text.setPosition(position);
				}
			}

			_texts.push_back(text);

			position.x += text.getLocalBounds().width + text.getCharacterSize() / 5;

			if (text.getLocalBounds().height > maxHeight)
			{
				maxHeight = text.getLocalBounds().height;
			}
		}

		position.y += maxHeight * 1.5f;
		position.x = baseX;
	}
}

void Text::draw(sf::RenderTarget& target, const sf::RenderStates states) const
{
	for (auto& text : _texts)
	{
		target.draw(text, states);
	}
}

void Text::SetColor(const sf::Color& color)
{
	for (auto& text : _texts)
	{
		text.setFillColor(color);
	}
}

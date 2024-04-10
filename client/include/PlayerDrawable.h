#pragma once

#include "SFML/Graphics.hpp"

enum class PlayerAnimation
{
	IDLE,
	WALK,
	JUMP
};

enum class PlayerDirection
{
	LEFT,
	RIGHT
};

/**
 * Used to manage player animations and sprite
 */
class PlayerDrawable : public sf::Drawable
{
 public:
	PlayerDrawable() = default;

 private:
	PlayerAnimation _animation = PlayerAnimation::IDLE;
	PlayerDirection _direction = PlayerDirection::RIGHT;
	sf::Vector2f _position = sf::Vector2f(0, 0);
	sf::Color _color = sf::Color::Cyan;
	int _frame = 0;
	sf::Time _elapsed = sf::Time::Zero;

	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

 public:
	void Update(sf::Time elapsed);

	void SetAnimation(PlayerAnimation animation);
	void SetDirection(PlayerDirection direction);
	void SetPosition(sf::Vector2f position);
	void SetColor(sf::Color color);
};
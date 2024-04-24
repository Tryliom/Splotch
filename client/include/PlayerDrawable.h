#pragma once

#include "Constants.h"

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
	PlayerRole _role = PlayerRole::PLAYER;
	PlayerAnimation _animation = PlayerAnimation::IDLE;
	PlayerDirection _direction = PlayerDirection::RIGHT;
	sf::Vector2f _position = sf::Vector2f(0, 0);
	sf::Color _color = sf::Color::Cyan;
	sf::Time _elapsed = sf::Time::Zero;
	int _frame = 0;
	bool _isLocalPlayer = false;

	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

 public:
	void Update(sf::Time elapsed);

	/**
	 * Set player role and color based on role
	 * @param role the player role
	 * @param isLocalPlayer true if the player is the local player
	 */
	void SetPlayerRole(PlayerRole role, bool isLocalPlayer);
	void SetAnimation(PlayerAnimation animation);
	void SetDirection(PlayerDirection direction);
	void SetPosition(sf::Vector2f position);
};
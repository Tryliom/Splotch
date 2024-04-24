#include "PlayerDrawable.h"

#include "Constants.h"
#include "AssetManager.h"

void PlayerDrawable::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	// Draw the player sprite
	sf::Sprite sprite;

	if (_role == PlayerRole::PLAYER)
	{
		sprite.setTextureRect(sf::IntRect(0, 0, PLAYER_SIZE.X, PLAYER_SIZE.Y));
		sprite.setOrigin(PLAYER_SIZE.X / 2.0f, PLAYER_SIZE.Y / 2.0f);
	}
	else
	{
		sprite.setTextureRect(sf::IntRect(0, 0, GHOST_SIZE.X, GHOST_SIZE.Y));
		sprite.setOrigin(GHOST_SIZE.X / 2.0f, GHOST_SIZE.Y / 2.0f);
	}

	sprite.setPosition(_position);
	sprite.setColor(_color);

	switch (_animation)
	{
	case PlayerAnimation::IDLE:
		if (_role == PlayerRole::PLAYER)
		{
			sprite.setTexture(AssetManager::GetPlayerIdleTexture(_frame));
		}
		else
		{
			sprite.setTexture(AssetManager::GetGhostIdleTexture(_frame));
		}
		break;
	case PlayerAnimation::WALK:
		sprite.setTexture(AssetManager::GetPlayerWalkTexture(_frame));
		break;
	case PlayerAnimation::JUMP:
		sprite.setTexture(AssetManager::GetPlayerJumpTexture(_frame));
		break;
	}

	if (_direction == PlayerDirection::LEFT)
	{
		sprite.setScale(-PLAYER_SIZE_SCALE, PLAYER_SIZE_SCALE);
	}
	else
	{
		sprite.setScale(PLAYER_SIZE_SCALE, PLAYER_SIZE_SCALE);
	}

	target.draw(sprite, states);
}

void PlayerDrawable::Update(sf::Time elapsed)
{
	_elapsed += elapsed;

	switch (_animation)
	{
	case PlayerAnimation::IDLE:
		if (_elapsed.asSeconds() >= PLAYER_IDLE_FRAME_DURATION)
		{
			_frame++;
			_elapsed -= sf::seconds(PLAYER_IDLE_FRAME_DURATION);
		}

		if (_frame >= PLAYER_IDLE_FRAMES) _frame = 0;
		break;
	case PlayerAnimation::WALK:
		if (_elapsed.asSeconds() >= PLAYER_WALK_FRAME_DURATION)
		{
			_frame++;
			_elapsed -= sf::seconds(PLAYER_WALK_FRAME_DURATION);
		}

		if (_frame >= PLAYER_WALK_FRAMES) _frame = 0;
		break;
	case PlayerAnimation::JUMP:
		if (_elapsed.asSeconds() >= PLAYER_JUMP_FRAME_DURATION)
		{
			_frame++;
			_elapsed -= sf::seconds(PLAYER_JUMP_FRAME_DURATION);
		}

		if (_frame >= PLAYER_JUMP_FRAMES) _frame = 0;
	}
}

void PlayerDrawable::SetPlayerRole(PlayerRole role, bool isLocalPlayer)
{
	static auto LOCAL_PLAYER_COLOR = sf::Color::Cyan;
	static auto REMOTE_PLAYER_COLOR = sf::Color::Red;

	_role = role;
	_color = isLocalPlayer ? LOCAL_PLAYER_COLOR : REMOTE_PLAYER_COLOR;
}

void PlayerDrawable::SetAnimation(PlayerAnimation animation)
{
	if (_animation == animation) return;
	if (_role == PlayerRole::GHOST && animation != PlayerAnimation::IDLE) return;

	_animation = animation;
	_frame = 0;
	_elapsed = sf::Time::Zero;
}

void PlayerDrawable::SetDirection(PlayerDirection direction)
{
	_direction = direction;
}

void PlayerDrawable::SetPosition(sf::Vector2f position)
{
	_position = position;
}
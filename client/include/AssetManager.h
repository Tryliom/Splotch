#pragma once

#include "Constants.h"

#include <SFML/Graphics.hpp>

constexpr auto PLAYER_IDLE_FRAMES = 2;
constexpr auto PLAYER_WALK_FRAMES = 7;
constexpr auto PLAYER_JUMP_FRAMES = 4;
constexpr auto GHOST_IDLE_FRAMES = 2;

constexpr float PLAYER_IDLE_FRAME_DURATION = 1.f / 6.f;
constexpr float PLAYER_WALK_FRAME_DURATION = 1.f / 20.f;
constexpr float PLAYER_JUMP_FRAME_DURATION = 1.f / 7.f;

namespace AssetManager
{
	/**
	 * @brief Initialize the AssetManager with all assets
	 */
	void Initialize();

	sf::Font& GetMainFont();
	/**
	 * @brief If frame is greater than PLAYER_IDLE_FRAMES, it will be set to frame % PLAYER_IDLE_FRAMES
	 */
	sf::Texture& GetPlayerIdleTexture(int frame);
	/**
	 * @brief If frame is greater than PLAYER_WALK_FRAMES, it will be set to frame % PLAYER_WALK_FRAMES
	 */
	sf::Texture& GetPlayerWalkTexture(int frame);
	/**
	 * @brief If frame is greater than PLAYER_JUMP_FRAMES, it will be set to frame % PLAYER_JUMP_FRAMES
	 */
	sf::Texture& GetPlayerJumpTexture(int frame);
	/**
	 * @brief If frame is greater than GHOST_IDLE_FRAMES, it will be set to frame % GHOST_IDLE_FRAMES
	 */
	sf::Texture& GetGhostIdleTexture(int frame);

	bool IsInitialized();
}
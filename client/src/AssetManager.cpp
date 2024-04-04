#include "AssetManager.h"

#include <array>

namespace AssetManager
{
	static sf::Font mainFont;
	static std::array<sf::Texture, PLAYER_IDLE_FRAMES> playerIdleTextures;
	static std::array<sf::Texture, PLAYER_WALK_FRAMES> playerWalkTextures;
	static std::array<sf::Texture, PLAYER_JUMP_FRAMES> playerJumpTextures;

	static bool initialized = false;

	void Initialize()
	{
		initialized = true;

		for (auto i = 0; i < PLAYER_IDLE_FRAMES; i++)
		{
			playerIdleTextures[i].loadFromFile("data/textures/player/idle_" + std::to_string(i) + ".png");
		}

		for (auto i = 0; i < PLAYER_WALK_FRAMES; i++)
		{
			playerWalkTextures[i].loadFromFile("data/textures/player/walk_" + std::to_string(i) + ".png");
		}

		for (auto i = 0; i < PLAYER_JUMP_FRAMES; i++)
		{
			playerJumpTextures[i].loadFromFile("data/textures/player/jump_" + std::to_string(i) + ".png");
		}

		mainFont.loadFromFile("data/font/Retro Gaming.ttf");
	}

	sf::Font& GetMainFont()
	{
		return mainFont;
	}

	sf::Texture& GetPlayerIdleTexture(int frame)
	{
		if (frame >= PLAYER_IDLE_FRAMES) frame %= PLAYER_IDLE_FRAMES;

		return playerIdleTextures[frame];
	}

	sf::Texture& GetPlayerWalkTexture(int frame)
	{
		if (frame >= PLAYER_WALK_FRAMES) frame %= PLAYER_WALK_FRAMES;

		return playerWalkTextures[frame];
	}

	sf::Texture& GetPlayerJumpTexture(int frame)
	{
		if (frame >= PLAYER_JUMP_FRAMES) frame %= PLAYER_JUMP_FRAMES;

		return playerJumpTextures[frame];
	}

	bool IsInitialized()
	{
		return initialized;
	}
}

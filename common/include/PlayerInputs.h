#pragma once

typedef std::uint8_t PlayerInput;

enum class PlayerInputTypes : std::uint8_t {
	Up = 1 << 0,
	Down = 1 << 1,
	Right = 1 << 2,
	Left = 1 << 3
};

inline bool IsKeyPressed(PlayerInput input, PlayerInputTypes key)
{
	return (static_cast<std::uint8_t>(input) & static_cast<std::uint8_t>(key)) != 0;
}

struct PlayerInputPerFrame
{
	int Frame;
	PlayerInput Input;
};
#pragma once

#include <cstdint>

typedef std::uint8_t PlayerInput;

enum class PlayerInputTypes : std::uint8_t {
	Up = 1u << 0u,
	Down = 1u << 1u,
	Right = 1u << 2u,
	Left = 1u << 3u
};

inline bool IsKeyPressed(PlayerInput input, PlayerInputTypes key)
{
	return (static_cast<std::uint8_t>(input) & static_cast<std::uint8_t>(key)) != 0;
}

struct PlayerInputPerFrame
{
	short Frame;
	PlayerInput Input;
};

struct FinalInputs
{
	PlayerInput PlayerRoleInput {};
	PlayerInput HandRoleInput {};
};
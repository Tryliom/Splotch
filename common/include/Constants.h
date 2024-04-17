#pragma once

#include "Vec2.h"

enum class HandSlot : char
{
	UNKNOWN = -1,
	SLOT_1 = 0,
	SLOT_2 = 1,
	SLOT_3 = 2,
	SLOT_4 = 3,
	SLOT_5 = 4,
	COUNT
};

constexpr char HAND_SLOT_COUNT = static_cast<char>(HandSlot::COUNT);

/**
 * Used to represent a value of the screen size
 */
struct ScreenSizeValue
{
	float Value;

	constexpr ScreenSizeValue operator*(ScreenSizeValue screenSizeValue) const
	{
		return { Value * screenSizeValue.Value };
	}

	constexpr ScreenSizeValue operator*(float value) const
	{
		return { Value * value };
	}

	constexpr ScreenSizeValue operator+(ScreenSizeValue screenSizeValue) const
	{
		return { Value + screenSizeValue.Value };
	}
};

/**
 * Used to represent a value of the screen size as a percentage
 */
struct ScreenPercentage
{
	float Value;

	constexpr float operator*(ScreenSizeValue screenSizeValue) const
	{
		return Value * screenSizeValue.Value;
	}

	constexpr ScreenPercentage operator*(int multiplier) const
	{
		return { Value * multiplier };
	}

	constexpr ScreenPercentage operator+(ScreenPercentage screenPercentage) const
	{
		return { Value + screenPercentage.Value };
	}

	constexpr ScreenPercentage operator-(ScreenPercentage screenPercentage) const
	{
		return { Value - screenPercentage.Value };
	}
};

using Vec2SP = Math::Vec2<ScreenPercentage>;

constexpr Vec2SP BRICK_SIZE = {{ 0.15f }, { 0.05f } };

constexpr Math::Vec2I PLAYER_SIZE = { 16, 16 };
constexpr float PLAYER_SIZE_SCALE = 3.f;
constexpr Math::Vec2I PLAYER_SIZE_SCALED = PLAYER_SIZE * PLAYER_SIZE_SCALE;
constexpr Math::Vec2I PLAYER_MIN_BOUND = -PLAYER_SIZE_SCALED / 2.f;
constexpr Math::Vec2I PLAYER_MAX_BOUND = PLAYER_SIZE_SCALED / 2.f;

constexpr Vec2SP HAND_START_POSITION = {{ 0.3f }, { 0.1f } };
constexpr ScreenPercentage HAND_SLOT_SIZE = { 0.1f };
constexpr ScreenPercentage MAX_HAND_BRICK_SPAWN_HEIGHT = { 1.f - 0.7f };

constexpr Vec2SP PLATFORM_MIN_BOUND = {{ -HAND_SLOT_SIZE.Value * 3.f }, { -0.01f }};
constexpr Vec2SP PLATFORM_MAX_BOUND = {{ HAND_SLOT_SIZE.Value * 3.f }, { 0.01}};
constexpr Vec2SP PLATFORM_SIZE = {{ HAND_SLOT_SIZE.Value * 5.f }, { PLATFORM_MAX_BOUND.Y * 2.f }};
constexpr Vec2SP PLATFORM_POSITION = {{ 0.5f }, { 0.95f } };

constexpr Vec2SP PLAYER_START_POSITION = {{ 0.5f }, { PLATFORM_POSITION.Y - PLATFORM_MAX_BOUND.Y } };

constexpr Math::Vec2F GRAVITY = { 0.f, 800.f };

constexpr int MAX_PLAYERS = 2;

enum class PlayerRole
{
	PLAYER,
	HAND
};

constexpr int FRAME_RATE = 30;
constexpr float TIME_PER_FRAME = 1.f / FRAME_RATE;
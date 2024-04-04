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

inline static constexpr char HAND_SLOT_COUNT = static_cast<char>(HandSlot::COUNT);

/**
 * Used to represent a value of the screen size
 */
struct ScreenSizeValue
{
	float Value;

	ScreenSizeValue operator*(ScreenSizeValue screenSizeValue) const
	{
		return { Value * screenSizeValue.Value };
	}

	ScreenSizeValue operator*(float value) const
	{
		return { Value * value };
	}

	ScreenSizeValue operator+(ScreenSizeValue screenSizeValue) const
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

	float operator*(ScreenSizeValue screenSizeValue) const
	{
		return Value * screenSizeValue.Value;
	}
};

using Vec2SP = Math::Vec2<ScreenPercentage>;

inline static constexpr Vec2SP BRICK_SIZE = {{ 0.15f }, { 0.05f } };

inline static constexpr Vec2SP PLATFORM_SIZE = {{ 0.75f }, { 0.05f } };
inline static constexpr Vec2SP PLATFORM_POSITION = {{ 0.5f }, { 0.9f } };

inline static constexpr Math::Vec2I PLAYER_SIZE = {16, 16 };
inline static constexpr Vec2SP PLAYER_START_POSITION = {{ 0.5f }, { 0.9f } };

inline static constexpr Vec2SP HAND_START_POSITION = {{ 0.25f }, { 0.9f } };
inline static constexpr ScreenPercentage HAND_SLOT_SIZE = { 0.15f };
inline static constexpr ScreenPercentage MAX_HAND_BRICK_SPAWN_HEIGHT = { 1.f - 0.7f };

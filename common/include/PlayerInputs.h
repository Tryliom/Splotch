#pragma once

#include <bitset>

struct PlayerInputs
{
	std::bitset<1> Up = 0;
	std::bitset<1> Down = 0;
	std::bitset<1> Left = 0;
	std::bitset<1> Right = 0;
};
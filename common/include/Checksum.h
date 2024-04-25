#pragma once

struct Checksum
{
	int Value = 0;

	bool operator==(const Checksum& other) const
	{
		return Value == other.Value;
	}
};
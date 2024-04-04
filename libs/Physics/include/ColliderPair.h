#pragma once

#include "Ref.h"

namespace Physics
{
    /**
     * @brief A pair of colliders
     */
	struct ColliderPair
	{
		ColliderRef A;
		ColliderRef B;

		bool operator==(const ColliderPair& other) const noexcept
		{
			return A == other.A && B == other.B || A == other.B && B == other.A;
		}
	};
}
#pragma once

#include <cstdlib>

namespace Physics
{
    /**
     * @brief A reference to a body or collider.
     * @tparam i The index of the reference. 0 for BodyRef, 1 for ColliderRef.
     */
    template<int i>
    struct Ref
    {
        std::size_t Index;
        std::size_t Generation;

        constexpr bool operator==(const Ref& other) const noexcept
        {
            return Index == other.Index && Generation == other.Generation;
        }

        constexpr bool operator!=(const Ref& other) const noexcept
        {
            return *this != other;
        }
    };

    /**
     * @brief A reference to a body.
     */
    using BodyRef = Ref<0>;
    /**
     * @brief A reference to a collider.
     */
    using ColliderRef = Ref<1>;
}
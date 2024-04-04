#pragma once

#include "Collider.h"
#include "ColliderPair.h"

#include "Allocator.h"

#include <vector>
#include <memory>

namespace Physics
{
	/**
	 * @brief A simplified collider that only contains the collider reference and the collider bounds (rectangle)
	 */
    struct SimplifiedCollider
    {
        ColliderRef Ref {};
        Math::RectangleF Bounds {Math::Vec2F::Zero(), Math::Vec2F::One()};
    };

	/**
	 * @brief A quadtree node that contains a boundary, a list of colliders and a boolean that indicates if the node has been divided
	 */
    struct QuadNode
    {
		explicit QuadNode(HeapAllocator& allocator) noexcept;

	    MyVector<SimplifiedCollider> Colliders;
        Math::RectangleF Boundary {Math::Vec2F::Zero(), Math::Vec2F::One()};
        bool Divided {false};
    };

	/**
	 * @brief A quadtree that contains a list of quadtree nodes and a list of all possible pairs of colliders
	 */
	class QuadTree
	{
	public:
        /**
         * @brief Preallocate quadtree nodes with 4^MaxDepth nodes
         * @param boundary The boundary of the quadtree
         */
		explicit QuadTree(const Math::RectangleF& boundary) noexcept;

	private:
		LinearAllocator _nodesAllocator;
		HeapAllocator _heapAllocator {};
		MyVector<QuadNode> _nodes { StandardAllocator <QuadNode> {_nodesAllocator} };
		MyVector<ColliderPair> _allPossiblePairs { StandardAllocator <ColliderPair> {_heapAllocator} };

        static constexpr std::size_t _maxDepth = 5;
		static constexpr std::size_t _maxCapacity = 8;

        static constexpr std::size_t getMaxNodes() noexcept;
        static constexpr std::size_t getDepth(std::size_t index) noexcept;

        void subdivide(std::size_t index) noexcept;
		void addAllPossiblePairs(std::size_t index, const SimplifiedCollider& collider) noexcept;

    public:
		/**
		 * @brief Insert a collider into the quadtree, if the quadtree is full, subdivide the quadtree and insert the collider into the correct node
		 * @param collider The collider to insert
		 * @param depth The current depth of the quadtree
		 */
		void Insert(SimplifiedCollider collider) noexcept;
		/**
		 * @brief Get all the possible pairs of colliders in the quadtree
		 * @return All the possible pairs of colliders in the quadtree
		 */
		[[nodiscard]] const MyVector<ColliderPair>& GetAllPossiblePairs() noexcept;

		/**
		 * @brief Set the new boundary of the quadtree, applies to all nodes
		 * @param boundary The new boundary
		 */
		void UpdateBoundary(const Math::RectangleF& boundary) noexcept;

		/**
		 * @brief Clear all colliders from the quadtree
		 */
		void ClearColliders() noexcept;

		/**
		 * @brief Get all the boundaries of the quadtree nodes that are not nullptr or empty
		 * @return All the boundaries of the quadtree nodes
		 */
		[[nodiscard]] std::vector<Math::RectangleF> GetBoundaries() const noexcept;
		/**
		 * @brief Get the number of colliders in the quadtree and all its nodes
		 * @return The number of colliders in the quadtree and all its nodes
		 */
		[[nodiscard]] std::size_t GetAllCollidersCount() const noexcept;

		static constexpr std::size_t MaxDepth() noexcept { return _maxDepth; }
		static constexpr std::size_t MaxCapacity() noexcept { return _maxCapacity; }
	};
}
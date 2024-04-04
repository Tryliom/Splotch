#include <cassert>
#include "Allocator.h"

#ifdef TRACY_ENABLE
#include <tracy/Tracy.hpp>
#endif

Allocator::Allocator(void* ptr, std::size_t size) noexcept :
    _rootPtr(ptr), _currentPtr(ptr), _size(size), _allocations(0) {}

void* Allocator::GetRootPtr() const noexcept
{
    return _rootPtr;
}

void* Allocator::GetCurrentPtr() const noexcept
{
    return _currentPtr;
}

std::size_t Allocator::GetSize() const noexcept
{
    return _size;
}

std::size_t Allocator::GetAllocations() const noexcept
{
    return _allocations;
}

std::size_t Allocator::calculateAlignForwardAdjustment(const void* address, std::size_t alignment)
{
	assert((alignment & (alignment - 1)) == 0 && "Alignment needs to be a power of two");

	const std::size_t adjustment = alignment - (reinterpret_cast<std::uintptr_t>(address) & (alignment - 1));

	if (adjustment == alignment) return 0;
	return adjustment;
}

std::size_t Allocator::calculateAlignForwardAdjustmentWithHeader(const void* address, std::size_t alignment, std::size_t headerSize)
{
	auto adjustment = calculateAlignForwardAdjustment(address, alignment);
	std::size_t neededSpace = headerSize;

	if (adjustment < neededSpace)
	{
		neededSpace -= adjustment;
		adjustment += alignment * (neededSpace / alignment);

		if (neededSpace % alignment > 0) adjustment += alignment;
	}

	return adjustment;
}

// LinearAllocator implementation

LinearAllocator::LinearAllocator(void* ptr, std::size_t size) noexcept : Allocator(ptr, size), _offset(0) {}

LinearAllocator::~LinearAllocator()
{
    std::free(_rootPtr);
}

void* LinearAllocator::Allocate(std::size_t size, std::size_t alignment) noexcept
{
	assert(size != 0 && "Linear Allocator cannot allocated nothing");
	const auto adjustment = calculateAlignForwardAdjustment(_currentPtr, alignment);

	assert(_offset + adjustment + size <= _size && "Linear Allocator has not enough space for this allocation");
	auto* alignedAddress = reinterpret_cast<void*>(reinterpret_cast<std::uintptr_t>(_currentPtr) + adjustment);

	_currentPtr = reinterpret_cast<void*>(reinterpret_cast<std::uintptr_t>(alignedAddress) + size);
	_offset += size + adjustment;
	_allocations++;

	return alignedAddress;
}

void LinearAllocator::Clear() noexcept
{
    _offset = 0;
    _currentPtr = _rootPtr;
    _allocations = 0;
}

// ProxyAllocator implementation

ProxyAllocator::ProxyAllocator(Allocator& allocator) noexcept :
    Allocator(allocator), _allocator(allocator) {}

void* ProxyAllocator::Allocate(std::size_t size, std::size_t alignment) noexcept
{
    auto ptr = _allocator.Allocate(size, alignment);

#ifdef TRACY_ENABLE
	TracyAlloc(ptr, size * alignment);
#endif

    // Update the number of allocations
    if (ptr != nullptr)
    {
        _allocations++;
    }

    return ptr;
}

void ProxyAllocator::Deallocate(void* ptr) noexcept
{
#ifdef TRACY_ENABLE
	TracyFree(ptr);
#endif

    _allocator.Deallocate(ptr);
}

void* HeapAllocator::Allocate(std::size_t size, std::size_t alignment) noexcept
{
	if (size == 0) return nullptr;

	const auto space = size * alignment;
	auto* ptr = std::malloc(space);

#ifdef TRACY_ENABLE
	TracyAlloc(ptr, space);
#endif

	return ptr;
}

void HeapAllocator::Deallocate(void* ptr) noexcept
{
	if (ptr == nullptr) return;

#ifdef TRACY_ENABLE
	TracyFree(ptr);
#endif

	std::free(ptr);
}

FreeListAllocator::FreeListAllocator(void* ptr, std::size_t size) noexcept
{
	_rootPtr = ptr;
	_size = size;
	_freeBlocks = static_cast<FreeBlock*>(_rootPtr);
	_freeBlocks->size = _size;
	_freeBlocks->next = nullptr;
	_currentPtr = _rootPtr;
	_allocations = 0;
}

void* FreeListAllocator::Allocate(std::size_t size, std::size_t alignment) noexcept
{
	assert(size != 0 && "FreeListAllocator cannot allocated nothing");

	FreeBlock* prevFreeBlock = nullptr;
	FreeBlock* freeBlock = _freeBlocks;

	while (freeBlock != nullptr)
	{
		const auto adjustment = calculateAlignForwardAdjustmentWithHeader(freeBlock, alignment, sizeof(AllocationHeader));
		auto totalSize = size + adjustment;

		if (freeBlock->size < totalSize)
		{
			prevFreeBlock = freeBlock;
			freeBlock = freeBlock->next;
			continue;
		}

		static_assert(sizeof(AllocationHeader) >= sizeof(FreeBlock), "sizeof(AllocationHeader) < sizeof(FreeBlock)");

		if (freeBlock->size - totalSize <= sizeof(AllocationHeader))
		{
			totalSize = freeBlock->size;

			if (prevFreeBlock != nullptr)
			{
				prevFreeBlock->next = freeBlock->next;
			}
			else
			{
				_freeBlocks = freeBlock->next;
			}
		}
		else
		{
			auto* nextBlock = reinterpret_cast<FreeBlock*>(reinterpret_cast<std::uintptr_t>(freeBlock) + totalSize);
			nextBlock->size = freeBlock->size - totalSize;
			nextBlock->next = freeBlock->next;

			if (prevFreeBlock != nullptr)
			{
				prevFreeBlock->next = nextBlock;
			}
			else
			{
				_freeBlocks = nextBlock;
			}
		}

		const auto alignedAddress = reinterpret_cast<void*>(reinterpret_cast<std::uintptr_t>(freeBlock) + adjustment);

		auto* header = reinterpret_cast<AllocationHeader*>(reinterpret_cast<std::uintptr_t>(alignedAddress) - sizeof(AllocationHeader));
		header->size = totalSize;
		header->adjustment = adjustment;

		_currentPtr = reinterpret_cast<void*>(reinterpret_cast<std::uintptr_t>(alignedAddress) + size);
		_allocations++;

#ifdef TRACY_ENABLE
		TracyAlloc(alignedAddress, size * alignment);
#endif

		return alignedAddress;
	}

	return nullptr;
}

void FreeListAllocator::Deallocate(void* ptr) noexcept
{
	if (ptr == nullptr) return;

	const auto* header = reinterpret_cast<const AllocationHeader*>(reinterpret_cast<std::uintptr_t>(ptr) - sizeof(AllocationHeader));
	const auto blockStart = reinterpret_cast<std::uintptr_t>(ptr) - header->adjustment;
	const auto blockSize = header->size;
	const auto blockEnd = blockStart + blockSize;

	FreeBlock* prevFreeBlock = nullptr;
	FreeBlock* freeBlock = _freeBlocks;

	while (freeBlock != nullptr)
	{
		if (reinterpret_cast<std::uintptr_t>(freeBlock) >= blockEnd) break;

		prevFreeBlock = freeBlock;
		freeBlock = freeBlock->next;
	}

	if (prevFreeBlock == nullptr)
	{
		prevFreeBlock = reinterpret_cast<FreeBlock*>(blockStart);
		prevFreeBlock->size = blockSize;
		prevFreeBlock->next = _freeBlocks;

		_freeBlocks = prevFreeBlock;
	}
	else if (reinterpret_cast<std::uintptr_t>(prevFreeBlock) + prevFreeBlock->size == blockStart)
	{
		prevFreeBlock->size += blockSize;
	}
	else
	{
		auto* temp = reinterpret_cast<FreeBlock*>(blockStart);
		temp->size = blockSize;
		temp->next = prevFreeBlock->next;

		prevFreeBlock->next = temp;
		prevFreeBlock = temp;
	}

	if (freeBlock != nullptr && reinterpret_cast<std::uintptr_t>(freeBlock) == blockEnd)
	{
		prevFreeBlock->size += freeBlock->size;
		prevFreeBlock->next = freeBlock->next;
	}

	_allocations--;

#ifdef TRACY_ENABLE
	TracyFree(ptr);
#endif
}

void FreeListAllocator::Clear() noexcept
{
	_freeBlocks = static_cast<FreeBlock*>(_rootPtr);
	_freeBlocks->size = _size;
	_freeBlocks->next = nullptr;

	_currentPtr = _rootPtr;
	_allocations = 0;
}
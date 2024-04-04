#pragma once

#include <cstddef>
#include <cstdlib>
#include <vector>

/**
 * @brief Allocator interface
 */
class Allocator
{
protected:
    /**
     * @brief Root pointer to allocated memory
     */
    void* _rootPtr {};
    /**
     * @brief Current pointer to allocated memory
     */
    void* _currentPtr {};
    /**
     * @brief Size of allocated memory
     */
    std::size_t _size {};
    /**
     * @brief Number of allocations
     */
    std::size_t _allocations {};

public:
    Allocator() = default;
    Allocator(void* ptr, std::size_t size) noexcept;
    virtual ~Allocator() = default;

    /**
     * @brief Allocate memory
     * @param size Size of memory to allocate
     * @return Pointer to allocated memory
     */
    [[nodiscard]] virtual void* Allocate(std::size_t size, std::size_t alignment) noexcept = 0;
    /**
     * @brief Deallocate memory
     * @param ptr Pointer to memory to deallocate
     */
    virtual void Deallocate(void* ptr) noexcept = 0;

    /**
     * @brief Get the root pointer
     */
    [[nodiscard]] void* GetRootPtr() const noexcept;
    /**
     * @brief Get the current pointer
     */
    [[nodiscard]] void* GetCurrentPtr() const noexcept;
    /**
     * @brief Get the size of allocated memory
     */
    [[nodiscard]] std::size_t GetSize() const noexcept;
    /**
     * @brief Get the number of allocations
     */
    [[nodiscard]] std::size_t GetAllocations() const noexcept;

protected:
	static std::size_t calculateAlignForwardAdjustment(const void* address, std::size_t alignment);
	static std::size_t calculateAlignForwardAdjustmentWithHeader(const void* address, std::size_t alignment, std::size_t headerSize);
};

/**
 * @brief Linear allocator
 */
class LinearAllocator final : public Allocator
{
private:
    std::size_t _offset = 0;

public:
    /**
     * @brief Constructor
     * @param size Size of memory to allocate
     */
    LinearAllocator(void* ptr, std::size_t size) noexcept;
    ~LinearAllocator() override;

    /**
     * @brief Allocate memory from allocator
     * @param size Size of memory to allocate
     * @return Pointer to allocated memory
     */
    [[nodiscard]] void* Allocate(std::size_t size, std::size_t alignment) noexcept override;
    /**
     * @brief Does nothing for linear allocator
     */
    void Deallocate(void* ptr) noexcept override {}

    /**
     * @brief Clear allocator
     */
    void Clear() noexcept;
};

/**
 * @brief Proxy allocator
 */
class ProxyAllocator final : public Allocator
{
private:
    Allocator& _allocator;

public:
    /**
     * @brief Constructor
     * @param allocator Allocator to proxy
     */
    explicit ProxyAllocator(Allocator& allocator) noexcept;
    ~ProxyAllocator() override = default;

    /**
     * @brief Allocate memory from allocator
     * @param size Size of memory to allocate
     * @return Pointer to allocated memory
     */
    [[nodiscard]] void* Allocate(std::size_t size, std::size_t alignment) noexcept override;
    /**
     * @brief Deallocate memory from allocator
     * @param ptr Pointer to memory to deallocate
     */
    void Deallocate(void* ptr) noexcept override;
};

class HeapAllocator final : public Allocator
{
public:
	/**
	 * @brief Allocate memory from allocator
	 * @param size Size of memory to allocate
	 * @return Pointer to allocated memory
	 */
	[[nodiscard]] void* Allocate(std::size_t size, std::size_t alignment) noexcept override;
	/**
	 * @brief Deallocate memory from allocator
	 * @param ptr Pointer to memory to deallocate
	 */
	void Deallocate(void* ptr) noexcept override;
};

struct AllocationHeader
{
	std::size_t size;
	std::size_t adjustment;
};

class FreeListAllocator final : public Allocator
{
	private:

	struct FreeBlock
	{
		std::size_t size {};
		FreeBlock* next {nullptr};
	};

	FreeBlock* _freeBlocks;

public:
	/**
	 * @brief Constructor, need to provide an extra size for the free block per allocation
	 * @param size Size of memory to allocate
	 */
	FreeListAllocator(void* ptr, std::size_t size) noexcept;
	~FreeListAllocator() override = default;

	/**
	 * @brief Allocate memory from allocator
	 * @param size Size of memory to allocate
	 * @return Pointer to allocated memory
	 */
	[[nodiscard]] void* Allocate(std::size_t size, std::size_t alignment) noexcept override;
	/**
	 * @brief Deallocate memory from allocator
	 * @param ptr Pointer to memory to deallocate
	 */
	void Deallocate(void* ptr) noexcept override;
	/**
	 * @brief Clear allocator
	 */
	void Clear() noexcept;
};

/**
 * \brief Custom proxy allocator respecting allocator_traits
 */
template<typename T>
class StandardAllocator final
{
public:
	typedef T value_type;
	explicit StandardAllocator(Allocator& allocator);
	template <class U>
	explicit StandardAllocator(const StandardAllocator<U>& allocator) noexcept : _allocator(allocator.GetAllocator()) {}
	T* allocate(std::size_t n);
	void deallocate(T* ptr, std::size_t n);
	[[nodiscard]] Allocator& GetAllocator() const { return _allocator; }
protected:
	Allocator& _allocator;
};

// Forced to define these things in .h because otherwise the linker complains about undefined symbols

template <class T, class U>
constexpr bool operator== (const StandardAllocator<T>&, const StandardAllocator<U>&) noexcept
{
	return true;
}

template <class T, class U>
constexpr bool operator!= (const StandardAllocator<T>&, const StandardAllocator<U>&) noexcept
{
	return false;
}

template <typename T>
StandardAllocator<T>::StandardAllocator(Allocator& allocator) : _allocator(allocator) {}

template <typename T>
T* StandardAllocator<T>::allocate(std::size_t n)
{
	return static_cast<T*>(_allocator.Allocate(n * sizeof(T), alignof(T)));
}

template <typename T>
void StandardAllocator<T>::deallocate(T* ptr, std::size_t n)
{
	_allocator.Deallocate(ptr);
}

template<typename T>
using MyVector = std::vector<T, StandardAllocator<T>>;
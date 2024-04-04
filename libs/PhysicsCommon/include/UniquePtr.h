#pragma once

#include <algorithm>

/**
 * @brief The UniquePtr class is a smart pointer that owns and manages another object through a pointer and disposes of that object when the UniquePtr goes out of scope.
 * @tparam T Type of the managed object.
 */
template<typename T>
class UniquePtr
{
public:
	UniquePtr() noexcept = default;

    constexpr explicit UniquePtr(T* ptr) noexcept
    {
        _ptr = ptr;
    }

    constexpr UniquePtr(UniquePtr&& other) noexcept
    {
        std::swap(_ptr, other._ptr);
    }

    constexpr UniquePtr& operator=(UniquePtr&& other) noexcept
    {
        std::swap(_ptr, other._ptr);

        return *this;
    }

    constexpr UniquePtr(const UniquePtr& other) noexcept = delete;
    constexpr UniquePtr& operator=(const UniquePtr& other) noexcept = delete;

    constexpr ~UniquePtr() noexcept
    {
        if (_ptr != nullptr)
        {
			delete _ptr;
            _ptr = nullptr;
        }
    }

private:
    T* _ptr = nullptr;

public:

    [[nodiscard]] constexpr T* operator->() noexcept
    {
        return _ptr;
    }

    [[nodiscard]] constexpr const T* operator->() const noexcept
    {
        return _ptr;
    }

    [[nodiscard]] constexpr T& operator*() noexcept
    {
        return *_ptr;
    }

    [[nodiscard]] constexpr const T& operator*() const noexcept
    {
        return *_ptr;
    }

    /**
     * @brief Returns a pointer to the managed object.
     * @return A pointer to the managed object.
     */
    constexpr T* Get() noexcept
    {
        return _ptr;
    }

    /**
     * @brief Returns a pointer to the managed object as a child type.
     * @tparam U Child type.
     * @return A pointer to the managed object as a child type.
     */
    template<typename U>
    constexpr UniquePtr<U> As() noexcept
    {
        return UniquePtr<U>(static_cast<U*>(_ptr));
    }
};

template<typename T>
constexpr UniquePtr<T> MakeUnique() noexcept
{
    return UniquePtr<T>(new T());
}

template<typename T, typename U>
constexpr UniquePtr<T> MakeUnique() noexcept
{
    return UniquePtr<T>(new U());
}
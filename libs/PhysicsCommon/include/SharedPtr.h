#pragma once

template<typename T>
class SharedPtr
{
public:
    constexpr explicit SharedPtr(T* ptr) noexcept
    {
        _ptr = ptr;
        _refCount = new int(1);
    }

    constexpr SharedPtr(SharedPtr&& other) noexcept
    {
        _ptr = other._ptr;
        _refCount = other._refCount;

        other._ptr = nullptr;
        other._refCount = nullptr;
    }

    constexpr SharedPtr(const SharedPtr& other) noexcept
    {
        _ptr = other._ptr;
        _refCount = other._refCount;

        (*_refCount)++;
    }

    constexpr SharedPtr& operator=(SharedPtr&& other) noexcept
    {
        _ptr = other._ptr;
        _refCount = other._refCount;

        other._ptr = nullptr;
        other._refCount = nullptr;

        return *this;
    }

    constexpr SharedPtr& operator=(const SharedPtr& other) noexcept
    {
        _ptr = other._ptr;
        _refCount = other._refCount;

        (*_refCount)++;

        return *this;
    }

    constexpr ~SharedPtr() noexcept
    {
        (*_refCount)--;

        if (*_refCount == 0 && _ptr != nullptr && _refCount != nullptr)
        {
            delete _ptr;
            delete _refCount;

            _ptr = nullptr;
            _refCount = nullptr;
        }
    }

private:
    T* _ptr = nullptr;
    int* _refCount = nullptr;

public:
    SharedPtr() noexcept = delete;

    constexpr T* Get() noexcept
    {
        return _ptr;
    }
};
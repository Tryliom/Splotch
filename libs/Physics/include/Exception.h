#pragma once

#include <exception>

/**
 * @brief Thrown when a body reference is invalid.
 */
class InvalidBodyRefException : public std::exception
{
public:
	InvalidBodyRefException() noexcept = default;
	~InvalidBodyRefException() noexcept override = default;

	[[nodiscard]] const char* what() const noexcept override
	{
		return "Invalid generation body reference";
	}
};

/**
 * @brief Thrown when a collider reference is invalid.
 */
class InvalidColliderRefException : public std::exception
{
public:
	InvalidColliderRefException() noexcept = default;
	~InvalidColliderRefException() noexcept override = default;

	[[nodiscard]] const char* what() const noexcept override
	{
		return "Invalid generation collider reference";
	}
};
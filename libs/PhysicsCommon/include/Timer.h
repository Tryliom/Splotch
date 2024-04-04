#pragma once

#include <chrono>

/**
 * @brief A simple timer class
 */
class Timer
{
public:
    Timer() noexcept;

private:
    std::chrono::duration<float> _deltaTime = std::chrono::duration<float>(0);
    std::chrono::time_point<std::chrono::high_resolution_clock> _currentTime;

public:
    /**
     * @brief Update the timer and calculate the delta time from the last frame
     */
	void Update() noexcept;

	/**
	 * @brief Get the Delta Time in seconds
	 */
	[[nodiscard]] constexpr float DeltaTime() const noexcept
	{
		return _deltaTime.count();
	}
};
#pragma once

#include "Renderer/Renderer.h"
#include "Constants.h"

#include <array>

class Application;

/**
 * @brief Renderer for the lobby screen used in Application
 */
class LobbyRenderer final : public Renderer
{
private:
	/**
	 * @brief Reference to the application, used to leave lobby
	 */
	Application& _application;

	/**
	 * @brief Height of the screen in pixels, used to position elements
	 */
	ScreenSizeValue _height;
	/**
	 * @brief Width of the screen in pixels, used to position elements
	 */
	ScreenSizeValue _width;

public:
	/**
	 * @brief Constructor, initializes the lobby screen elements
	 * @param game Reference to the application
	 * @param width Width of the screen in pixels
	 * @param height Height of the screen in pixels
	 */
	explicit LobbyRenderer(Application& game, ScreenSizeValue width, ScreenSizeValue height);

	void OnEvent(Event event) override;
};
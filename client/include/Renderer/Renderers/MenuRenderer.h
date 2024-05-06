#pragma once

#include "Renderer/Renderer.h"
#include "Constants.h"

#include <array>

class Application;

/**
 * @brief Renderer for the menu screen used in Application
 */
class MenuRenderer final : public Renderer
{
private:
	/**
	 * @brief Reference to the application, used to join the lobby
	 */
	Application& _application;

public:
	/**
	 * @brief Construct a new Menu Renderer object
	 *
	 * @param game Reference to the application
	 * @param width Width of the screen in pixels
	 * @param height Height of the screen in pixels
	 */
	explicit MenuRenderer(Application& game, ScreenSizeValue width, ScreenSizeValue height);

	void OnEvent(Event event) override;
};
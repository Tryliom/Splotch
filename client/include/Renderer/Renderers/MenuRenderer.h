#pragma once

#include "Renderer/Renderer.h"
#include "Constants.h"

#include <array>

class Application;

class MenuRenderer final : public Renderer
{
private:
	Application& _application;

public:
	explicit MenuRenderer(Application& game, ScreenSizeValue width, ScreenSizeValue height);

	void OnEvent(Event event) override;
};
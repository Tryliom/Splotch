#pragma once

#include "Renderer/Renderer.h"
#include "Constants.h"

#include <array>

class Application;

class LobbyRenderer final : public Renderer
{
private:
	Application& _game;

	ScreenSizeValue _height;
	ScreenSizeValue _width;

public:
	explicit LobbyRenderer(Application& game, ScreenSizeValue width, ScreenSizeValue height);

	void OnEvent(Event event) override;
};
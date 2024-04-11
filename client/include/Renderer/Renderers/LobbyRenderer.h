#pragma once

#include "Renderer/Renderer.h"
#include "Constants.h"

#include <array>

class Game;

class LobbyRenderer final : public Renderer
{
private:
	Game& _game;

	ScreenSizeValue _height;
	ScreenSizeValue _width;

public:
	explicit LobbyRenderer(Game& game, ScreenSizeValue width, ScreenSizeValue height);

	void OnPacketReceived(Packet& packet) override;
};
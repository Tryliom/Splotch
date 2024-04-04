#pragma once

#include "gui/Gui.h"
#include "Constants.h"

#include <array>

class Game;

class LobbyGui final : public Gui
{
private:
	Game& _game;

	ScreenSizeValue _height;
	ScreenSizeValue _width;

public:
	explicit LobbyGui(Game& game, ScreenSizeValue width, ScreenSizeValue height);

	void OnPacketReceived(Packet& packet) override;
};
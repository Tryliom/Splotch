#pragma once

#include "gui/Gui.h"
#include "Constants.h"

#include <array>

class Game;
class GameManager;

class MenuGui final : public Gui
{
public:
	explicit MenuGui(Game& game, ScreenSizeValue width, ScreenSizeValue height);
};
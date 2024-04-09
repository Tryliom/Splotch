#pragma once

#include "gui/Gui.h"
#include "Constants.h"

#include <array>

class Game;
class GameManager;

class MenuGui final : public Gui
{
private:
	Game& _game;

public:
	explicit MenuGui(Game& game, ScreenSizeValue width, ScreenSizeValue height);
	void OnUpdate(sf::Time elapsed, sf::Vector2f mousePosition) override;
};
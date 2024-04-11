#pragma once

#include "Renderer/Renderer.h"
#include "Constants.h"

#include <array>

class Game;
class GameManager;

class MenuRenderer final : public Renderer
{
private:
	Game& _game;

public:
	explicit MenuRenderer(Game& game, ScreenSizeValue width, ScreenSizeValue height);
	void OnUpdate(sf::Time elapsed, sf::Vector2f mousePosition) override;
};
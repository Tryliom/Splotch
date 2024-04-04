#pragma once

#include "Packet.h"
#include "Constants.h"
#include "Gui/Gui.h"
#include "GameManager.h"

#include "ClientNetworkInterface.h"

#include <SFML/Graphics.hpp>

#include <array>

enum class GameState
{
	NONE,
	MAIN_MENU,
	LOBBY,
	GAME
};

class Game
{
 public:
	explicit Game(GameManager& gameManager, ClientNetworkInterface& networkManager, ScreenSizeValue width, ScreenSizeValue height);

	void CheckInputs(const sf::Event& event);
	void Update(sf::Time elapsed, sf::Vector2f mousePosition);
	void SetState(GameState state);
	void Draw(sf::RenderTarget& target);

	void SendPacket(Packet* packet);
	void Quit();

	void OnQuit(std::function<void()> onQuit);

 private:
	std::function<void()> _onQuit;
	// Gui
	Gui* _gui { nullptr };
	// Game
	GameManager& _gameManager;
	ClientNetworkInterface& _networkManager;

	GameState _state = GameState::NONE;

	ScreenSizeValue _width;
	ScreenSizeValue _height;

	void OnPacketReceived(Packet& packet);
};
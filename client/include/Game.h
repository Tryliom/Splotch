#pragma once

#include "Packet.h"
#include "Constants.h"
#include "Gui/Gui.h"
#include "GameManager.h"
#include "RollbackManager.h"

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
	explicit Game(RollbackManager& rollbackManager, GameManager& gameManager, ClientNetworkInterface& networkManager, ScreenSizeValue width, ScreenSizeValue height);

	void CheckInputs(const sf::Event& event);
	void OnPlayerInput(PlayerInput playerInput);
	/**
	 * @brief Used to update the game at a fixed rate, for physics calculations
	 * @param elapsed
	 */
	void FixedUpdate(sf::Time elapsed);
	/**
	 * @brief Used to update the game at a variable rate, for rendering
	 * @param elapsed
	 * @param mousePosition
	 */
	void Update(sf::Time elapsed, sf::Vector2f mousePosition);
	void SetState(GameState state);
	void Draw(sf::RenderTarget& target);

	void SendPacket(Packet* packet, Protocol protocol);
	void Quit();

	void OnQuit(std::function<void()> onQuit);

	[[nodiscard]] bool IsReadyToPlay() const { return _readyToPlay; }

 private:
	std::function<void()> _onQuit;
	// Gui
	Gui* _gui { nullptr };
	// Game
	RollbackManager& _rollbackManager;
	GameManager& _gameManager;
	ClientNetworkInterface& _networkManager;

	GameState _state = GameState::NONE;

	ScreenSizeValue _width;
	ScreenSizeValue _height;

	bool _readyToPlay = false;
	static constexpr float _timeBeforeSendUdpAck = 1.0f;
	sf::Time _elapsedTime = sf::seconds(_timeBeforeSendUdpAck);

	void OnPacketReceived(Packet& packet);
};
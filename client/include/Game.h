#pragma once

#include "Packet.h"
#include "Constants.h"
#include "Renderer/Renderer.h"
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
	/**
	 * @brief Register the player input into rollback manager and send it to the server
	 * @param playerInput
	 */
	void RegisterPlayerInput(PlayerInput playerInput);
	/**
	 * @brief Used to update the game at a fixed rate, for physics calculations
	 * @param elapsed
	 */
	void FixedUpdate(sf::Time elapsed);
	/**
	 * @brief Used to update the game at a variable rate, for rendering
	 * @param elapsed
	 * @param elapsedSinceLastFixed Time since the last fixed update
	 * @param mousePosition
	 */
	void Update(sf::Time elapsed, sf::Time elapsedSinceLastFixed, sf::Vector2f mousePosition);

	void SetState(GameState state);
	void Draw(sf::RenderTarget& target);

	void SendPacket(Packet* packet, Protocol protocol);
	void Quit();

	void OnQuit(std::function<void()> onQuit);

	[[nodiscard]] bool IsReadyToPlay() const { return _readyToPlay; }
	[[nodiscard]] GameState GetState() const { return _state;}

 private:
	std::function<void()> _onQuit;
	// Renderer
	Renderer* _renderer { nullptr };
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
	void UpdateGame(sf::Time elapsed, short frame);
};
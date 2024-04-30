#pragma once

#include "Renderer/Renderer.h"
#include "Packet.h"
#include "Constants.h"
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

class Application
{
 public:
	explicit Application(RollbackManager& rollbackManager, GameManager& gameManager, ClientNetworkInterface& networkManager,
			ScreenSizeValue width, ScreenSizeValue height);

	void OnInput(const sf::Event& event);
	/**
	 * @brief Save local player input from current frame and send it to the server
	 * @param playerInput
	 */
	void AddLocalPlayerInput(PlayerInput playerInput);
	/**
	 * @brief Used to update the game at a fixed rate, for physics calculations, use FIXED_TIME_STEP
	 */
	void FixedUpdate();
	/**
	 * @brief Used to update the game at a variable rate, for rendering
	 * @param elapsed
	 * @param elapsedSinceLastFixed Time since the last fixed update
	 * @param mousePosition
	 */
	void Update(sf::Time elapsed, sf::Time elapsedSinceLastFixed, sf::Vector2f mousePosition);

	void Draw(sf::RenderTarget& target);

	void LeaveLobby();
	void LeaveGame();
	void StartGame();
	void JoinLobby();
	void Quit();

	[[nodiscard]] bool IsRunning() const { return _running; }

 private:
	std::function<void()> _onQuit;
	// Renderer
	Renderer* _renderer { nullptr };
	// Application
	RollbackManager& _rollbackManager;
	GameManager& _gameManager;
	ClientNetworkInterface& _networkManager;

	GameState _state = GameState::NONE;

	ScreenSizeValue _width;
	ScreenSizeValue _height;

	bool _readyToPlay = false;
	static constexpr float _timeBeforeSendUdpAck = 1.0f;
	sf::Time _elapsedTime = sf::seconds(_timeBeforeSendUdpAck);

	bool _running { true };

	void OnPacketReceived(Packet& packet);
	void UpdateGame(int frame);
	void SetState(GameState state);

	static void IntegrityLog();
};

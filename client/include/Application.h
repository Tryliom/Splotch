#pragma once

#include "Renderer/Renderer.h"
#include "Packet.h"
#include "Constants.h"
#include "GameManager.h"
#include "RollbackManager.h"

#include "ClientNetworkInterface.h"

#include <SFML/Graphics.hpp>

#include <array>

/**
 * @brief Enum class representing the different states of the game, used to determine what to render and update
 */
enum class GameState
{
	NONE,
	MAIN_MENU,
	LOBBY,
	GAME
};

/**
 * @brief Main class of the client, handles the game loop and the game state
 */
class Application
{
 public:
	/**
	 * @brief Construct a new Application object, change state to MAIN_MENU at the start
	 * @param rollbackManager RollbackManager used to rollback the game state
	 * @param gameManager GameManager used to manage the game state
	 * @param networkManager ClientNetworkInterface used to send and receive packets
	 * @param width The width of the window in pixels
	 * @param height The height of the window in pixels
	 */
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

	/**
	 * @brief Used to render the game
	 * @param target The target to render to
	 */
	void Draw(sf::RenderTarget& target);

	/**
	 * @brief Leave the lobby, change state to MAIN_MENU and send a LeaveLobbyPacket
	 */
	void LeaveLobby();
	/**
	 * @brief Leave the game, change state to MAIN_MENU and send a LeaveGamePacket
	 */
	void LeaveGame();
	/**
	 * @brief Start the game, change state to GAME
	 */
	void StartGame();
	/**
	 * @brief Join the lobby, change state to LOBBY and send a JoinLobbyPacket
	 */
	void JoinLobby();
	/**
	 * @brief Quit the game
	 */
	void Quit();

	/**
	 * @brief Check if the game is running
	 * @return true if the game is running
	 */
	[[nodiscard]] bool IsRunning() const { return _running; }

 private:
	/**
	 * @brief Renderer used to render the application
	 */
	Renderer* _renderer { nullptr };
	/**
	 * @brief RollbackManager used to rollback the game state
	 */
	RollbackManager& _rollbackManager;
	/**
	 * @brief GameManager used to manage the game state
	 */
	GameManager& _gameManager;
	/**
	 * @brief ClientNetworkInterface used to send and receive packets
	 */
	ClientNetworkInterface& _networkManager;
	/**
	 * @brief Current state of the game, used to determine what to render and update
	 */
	GameState _state = GameState::NONE;
	/**
	 * @brief Width of the window in pixels
	 */
	ScreenSizeValue _width;
	/**
	 * @brief Height of the window in pixels
	 */
	ScreenSizeValue _height;

	/**
	 * @brief Bool used to determine if the client is ready to play, used to send an AckPacket to the server
	 */
	bool _readyToPlay = false;
	/**
	 * @brief Time before sending an AckPacket to the server
	 */
	static constexpr float _timeBeforeSendUdpAck = 1.0f;
	/**
	 * @brief Time since the last AckPacket was sent
	 */
	sf::Time _elapsedTime = sf::seconds(_timeBeforeSendUdpAck);

	/**
	 * @brief Bool used to determine if the game is running
	 */
	bool _running { true };

	/**
	 * @brief Handle a packet received from the server
	 * @param packet The packet received
	 */
	void OnPacketReceived(Packet& packet);
	/**
	 * @brief Update the game state at a fixed rate
	 * @param frame A frame number used to update the game state
	 */
	void UpdateGame(int frame);
	/**
	 * @brief Set the state of the game
	 * @param state The state to set
	 */
	void SetState(GameState state);
};

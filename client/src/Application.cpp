#include "Application.h"

#include "Renderer/Renderers/MenuRenderer.h"
#include "Renderer/Renderers/LobbyRenderer.h"
#include "Renderer/Renderers/GameRenderer.h"

#include "MyPackets.h"
#include "MyPackets/JoinLobbyPacket.h"
#include "MyPackets/PlayerInputPacket.h"
#include "MyPackets/LeaveGamePacket.h"
#include "MyPackets/LeaveLobbyPacket.h"

#include <SFML/Graphics.hpp>
#include <utility>

Application::Application(RollbackManager& rollbackManager, GameManager& gameManager, ClientNetworkInterface& clientNetworkInterface, ScreenSizeValue width, ScreenSizeValue height) :
	_rollbackManager(rollbackManager), _gameManager(gameManager), _networkManager(clientNetworkInterface), _width(width), _height(height)
{
	SetState(GameState::MAIN_MENU);
}

void Application::OnInput(const sf::Event& event)
{
	if (_renderer == nullptr) return;

	_renderer->Input(event);
}

void Application::AddLocalPlayerInput(PlayerInput playerInput)
{
	if (_state != GameState::GAME || _gameManager.GetGameData().IsGameOver()) return;

	// Save the local player input for the current frame and send it to the server
	_rollbackManager.AddPlayerInputs(playerInput);
	_networkManager.SendPacket(new MyPackets::PlayerInputPacket(_rollbackManager.GetLastLocalPlayerInputs()), Protocol::UDP);
}

void Application::FixedUpdate()
{
	sf::Time elapsed = sf::seconds(FIXED_TIME_STEP);

	// Process all packets received from the server since the last fixed update
	while (Packet* packet = _networkManager.PopPacket())
	{
		// Special case for UDP connection confirmation
		if (packet->Type == static_cast<char>(PacketType::ConfirmUDPConnection))
		{
			_readyToPlay = true;
			if (_renderer != nullptr) _renderer->OnEvent(Event::READY_TO_PLAY);
			delete packet;
			continue;
		}

		// Dispatch the packet to the rollback manager, game manager and the application
		_rollbackManager.OnPacketReceived(*packet);
		_gameManager.OnPacketReceived(*packet);
		OnPacketReceived(*packet);

		// Delete the packet if it is a valid packet
		auto packetTypeValue = packet->Type;
		if (packetTypeValue >= 0 && packetTypeValue <= static_cast<char>(MyPackets::MyPacketType::COUNT))
		{
			delete packet;
		}
	}

	if (_state == GameState::GAME && _renderer != nullptr)
	{
		// Rollback if needed before updating the game
		if (_rollbackManager.NeedToRollback())
		{
			// The confirmed frame of the last set of confirmed game data
			const auto oldConfirmedFrame = _rollbackManager.GetConfirmedFrame();
			const auto confirmedInputFrame = _rollbackManager.GetConfirmedInputFrame();
			const auto currentFrame = _rollbackManager.GetCurrentFrame();

			_gameManager.SetGameData(_rollbackManager.GetConfirmedGameData());
			_rollbackManager.ResetUnconfirmedGameData();

			// Rollback before the current frame
			for (auto frame = oldConfirmedFrame; frame < currentFrame; frame++)
			{
				UpdateGame(frame);
				_gameManager.UpdatePlayerAnimations(elapsed, sf::seconds(0));

				// Happens when rollback is happening by reception of a confirmed input without remote inputs
				// So, it needs to update the confirmed game data when validating the confirmed input
				if (frame < confirmedInputFrame)
				{
					_rollbackManager.SetConfirmedGameData(_gameManager.GetGameData());
					_rollbackManager.CheckIntegrity(frame);
				}
				else
				{
					_rollbackManager.AddUnconfirmedGameData(_gameManager.GetGameData());
				}
			}

			_rollbackManager.RollbackDone();
		}

		// Update the game with the current frame
		UpdateGame(_rollbackManager.GetCurrentFrame());
		_rollbackManager.AddUnconfirmedGameData(_gameManager.GetGameData());

		// Check if the game is over
		if (_gameManager.GetGameData().BricksLeft == 0)
		{
			_renderer->OnEvent(_gameManager.GetLocalPlayerRole() == PlayerRole::PLAYER ? Event::WIN_GAME : Event::LOSE_GAME);
		}
	}
}

void Application::Update(sf::Time elapsed, sf::Time elapsedSinceLastFixed, sf::Vector2f mousePosition)
{
	if (_renderer != nullptr)
	{
		_renderer->Update(elapsed, elapsedSinceLastFixed, mousePosition);
	}

	// Send a UDP acknowledgment packet every _timeBeforeSendUdpAck seconds
	if (!_readyToPlay)
	{
		_elapsedTime += elapsed;

		if (_elapsedTime.asSeconds() >= _timeBeforeSendUdpAck)
		{
			_networkManager.SendUDPAcknowledgmentPacket();
			_elapsedTime = sf::Time::Zero;
		}
	}
}

void Application::SetState(GameState state)
{
	if (_state == state) return;

	delete _renderer;

	switch (state)
	{
		case GameState::MAIN_MENU: _renderer = new MenuRenderer(*this, _width, _height); break;
		case GameState::LOBBY: _renderer = new LobbyRenderer(*this, _width, _height); break;
		case GameState::GAME: _renderer = new GameRenderer(*this, _gameManager, _width, _height); break;
		default: break;
	}

	_state = state;
}

void Application::Draw(sf::RenderTarget& target)
{
	// Draw white background
	sf::RectangleShape background(sf::Vector2f(_width.Value, _height.Value));
	background.setFillColor(sf::Color::White);
	target.draw(background);

	if (_renderer != nullptr)
	{
		target.draw(*_renderer);
	}
}

void Application::LeaveLobby()
{
	SetState(GameState::MAIN_MENU);
	_networkManager.SendPacket(new MyPackets::LeaveLobbyPacket(), Protocol::TCP);
}

void Application::LeaveGame()
{
	SetState(GameState::MAIN_MENU);
	_networkManager.SendPacket(new MyPackets::LeaveGamePacket(), Protocol::TCP);
}

void Application::StartGame()
{
	SetState(GameState::GAME);
}

void Application::JoinLobby()
{
	SetState(GameState::LOBBY);
	_networkManager.SendPacket(new MyPackets::JoinLobbyPacket(), Protocol::TCP);
}

void Application::OnPacketReceived(Packet& packet)
{
	if (_renderer == nullptr) return;

	if (packet.Type == static_cast<char>(MyPackets::MyPacketType::LeaveGame))
	{
		_renderer->OnEvent(Event::PLAYER_LEAVE_GAME);
	}
	else if (packet.Type == static_cast<char>(MyPackets::MyPacketType::StartGame))
	{
		_renderer->OnEvent(Event::START_GAME);
	}
}

void Application::Quit()
{
	_running = false;
}

void Application::UpdateGame(int frame)
{
	const auto previousFrame = frame - 1;
	const auto currentPlayer1Inputs = _rollbackManager.GetPlayerInput(PlayerNumber::PLAYER1, frame);
	const auto previousPlayer1Inputs = _rollbackManager.GetPlayerInput(PlayerNumber::PLAYER1, previousFrame);
	const auto currentPlayer2Inputs = _rollbackManager.GetPlayerInput(PlayerNumber::PLAYER2, frame);
	const auto previousPlayer2Inputs = _rollbackManager.GetPlayerInput(PlayerNumber::PLAYER2, previousFrame);

	_gameManager.FixedUpdate(currentPlayer1Inputs, previousPlayer1Inputs, currentPlayer2Inputs, previousPlayer2Inputs);
}
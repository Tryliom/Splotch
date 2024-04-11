#include "Game.h"


#include "Renderer/Renderers/MenuRenderer.h"
#include "Renderer/Renderers/LobbyRenderer.h"
#include "Renderer/Renderers/GameRenderer.h"

#include "MyPackets.h"
#include "MyPackets/JoinLobbyPacket.h"
#include "MyPackets/PlayerInputPacket.h"

#include <SFML/Graphics.hpp>
#include <utility>

Game::Game(RollbackManager& rollbackManager, GameManager& gameManager, ClientNetworkInterface& clientNetworkInterface, ScreenSizeValue width, ScreenSizeValue height) :
	_rollbackManager(rollbackManager), _gameManager(gameManager), _networkManager(clientNetworkInterface), _width(width), _height(height)
{
	SetState(GameState::MAIN_MENU);
}

void Game::CheckInputs(const sf::Event& event)
{
	if (_renderer != nullptr)
	{
		_renderer->CheckInputs(event);
	}
}

void Game::OnPlayerInput(PlayerInput playerInput)
{
	if (_state != GameState::GAME) return;

	_rollbackManager.AddPlayerInputs(playerInput);
}

void Game::FixedUpdate(sf::Time elapsed)
{
	while (Packet* packet = _networkManager.PopPacket())
	{
		if (packet->Type == static_cast<char>(PacketType::ConfirmUDPConnection))
		{
			_readyToPlay = true;
			delete packet;
			continue;
		}

		_rollbackManager.OnPacketReceived(*packet);
		_gameManager.OnPacketReceived(*packet);
		OnPacketReceived(*packet);

		auto packetTypeValue = packet->Type;

		if (packetTypeValue >= 0 && packetTypeValue <= static_cast<char>(MyPackets::MyPacketType::COUNT))
		{
			delete packet;
		}
	}

	if (_state == GameState::GAME)
	{
		const auto playerRole = _gameManager.GetPlayerRole();

		_gameManager.SetPlayerInputs(_rollbackManager.GetLastPlayerInput(playerRole));
		_gameManager.SetHandInputs(_rollbackManager.GetLastHandInput(playerRole));

		if (_gameManager.GetPlayerInputs() == _gameManager.GetHandInputs() && _gameManager.GetPlayerInputs() != 0)
		{
			_gameManager.SetPlayerInputs(_rollbackManager.GetLastPlayerInput(playerRole));
			_gameManager.SetHandInputs(_rollbackManager.GetLastHandInput(playerRole));
		}

		SendPacket(new MyPackets::PlayerInputPacket(_rollbackManager.GetLastPlayerInputs()), Protocol::UDP);
	}

	if (_renderer != nullptr)
	{
		_renderer->FixedUpdate(elapsed);
	}
}

void Game::Update(sf::Time elapsed, sf::Vector2f mousePosition)
{
	if (_renderer != nullptr)
	{
		_renderer->Update(elapsed, mousePosition);
	}

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

void Game::SetState(GameState state)
{
	if (_state == state) return;

	delete _renderer;

	if (state == GameState::MAIN_MENU)
	{
		_renderer = new MenuRenderer(*this, _width, _height);
	}

	if (state == GameState::LOBBY)
	{
		_renderer = new LobbyRenderer(*this, _width, _height);
		_networkManager.SendPacket(new MyPackets::JoinLobbyPacket(), Protocol::TCP);
	}

	if (state == GameState::GAME)
	{
		_renderer = new GameRenderer(*this, _gameManager, _width, _height);
	}

	_state = state;
}

void Game::Draw(sf::RenderTarget& target)
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

void Game::SendPacket(Packet* packet, Protocol protocol)
{
	_networkManager.SendPacket(packet, protocol);
}

void Game::OnPacketReceived(Packet& packet)
{
	if (_renderer != nullptr)
	{
		_renderer->OnPacketReceived(packet);
	}
}

void Game::Quit()
{
	if (_onQuit != nullptr)
	{
		_onQuit();
	}
}

void Game::OnQuit(std::function<void()> onQuit)
{
	_onQuit = std::move(onQuit);
}
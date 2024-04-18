#include "GameManager.h"

#include "MyPackets.h"
#include "MyPackets/StartGamePacket.h"

GameManager::GameManager(ScreenSizeValue width, ScreenSizeValue height) : _width(width), _height(height) {}

void GameManager::OnPacketReceived(Packet& packet)
{
	if (packet.Type == static_cast<char>(MyPackets::MyPacketType::StartGame))
	{
		auto& startGamePacket = *packet.As<MyPackets::StartGamePacket>();

		_playerRole = startGamePacket.IsPlayer ? PlayerRole::PLAYER : PlayerRole::HAND;
		_gameData.StartGame(_width, _height);

		_gameData.Players[0].SetColor(startGamePacket.IsPlayer ? sf::Color::Cyan : sf::Color::Red);
		_gameData.Players[1].SetColor(startGamePacket.IsPlayer ? sf::Color::Red : sf::Color::Cyan);
	}
}

PlayerRole GameManager::GetPlayerRole()
{
	return _playerRole;
}

void GameManager::Update(sf::Time elapsed,
	PlayerInput playerInput, PlayerInput previousPlayerInput,
	PlayerInput handInput, PlayerInput previousHandInput)
{
	_gameData.RegisterPlayersInputs(playerInput, previousPlayerInput, handInput, previousHandInput);
	_gameData.Update(elapsed);
}

ClientGameData GameManager::GetGameData() const
{
	return _gameData;
}

void GameManager::SetGameData(ClientGameData gameData)
{
	_gameData = std::move(gameData);
}

void GameManager::UpdatePlayerAnimations(sf::Time elapsed, sf::Time elapsedSinceLastFixed)
{
	_gameData.UpdatePlayersAnimations(elapsed, elapsedSinceLastFixed);
}

#include "GameManager.h"

#include "MyPackets.h"
#include "MyPackets/StartGamePacket.h"

GameManager::GameManager(ScreenSizeValue width, ScreenSizeValue height) : _width(width), _height(height) {}

void GameManager::OnPacketReceived(Packet& packet)
{
	if (packet.Type == static_cast<char>(MyPackets::MyPacketType::StartGame))
	{
		auto& startGamePacket = *packet.As<MyPackets::StartGamePacket>();

		_gameData.SetLocalPlayerRole(startGamePacket.IsPlayer ? PlayerRole::PLAYER : PlayerRole::GHOST, startGamePacket.IsFirstNumber);
		_gameData.StartGame(_width, _height);
	}
}

PlayerRole GameManager::GetLocalPlayerRole() const
{
	return _gameData.LocalPlayerRole;
}

void GameManager::FixedUpdate(PlayerInput player1Input, PlayerInput player1PreviousInput, PlayerInput player2Input, PlayerInput player2PreviousInput)
{
	_gameData.SetInputs(player1Input, player1PreviousInput, player2Input, player2PreviousInput);
	_gameData.FixedUpdate();
}

ClientGameData GameManager::GetGameData() const
{
	return _gameData;
}

void GameManager::SetGameData(ClientGameData gameData)
{
	_gameData = std::move(gameData);
	_gameData.World.SetContactListener(&_gameData);
}

void GameManager::UpdatePlayerAnimations(sf::Time elapsed, sf::Time elapsedSinceLastFixed)
{
	_gameData.UpdatePlayersAnimations(elapsed, elapsedSinceLastFixed);
}
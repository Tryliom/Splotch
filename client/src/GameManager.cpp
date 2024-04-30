#include "GameManager.h"

#include "MyPackets.h"
#include "MyPackets/StartGamePacket.h"

GameManager::GameManager(ScreenSizeValue width, ScreenSizeValue height) : _width(width), _height(height) {}

void GameManager::OnPacketReceived(Packet& packet)
{
	if (packet.Type == static_cast<char>(MyPackets::MyPacketType::StartGame))
	{
		auto& startGamePacket = *packet.As<MyPackets::StartGamePacket>();

		_playerRole = startGamePacket.IsPlayer ? PlayerRole::PLAYER : PlayerRole::GHOST;
		_gameData.StartGame(_width, _height);
		_gameData.SetPlayersRole(startGamePacket.IsPlayer);
	}
}

PlayerRole GameManager::GetPlayerRole()
{
	return _playerRole;
}

void GameManager::Update(PlayerInput playerInput, PlayerInput previousPlayerInput, PlayerInput ghostInput, PlayerInput previousGhostInput)
{
	_gameData.AddPlayersInputs(playerInput, previousPlayerInput, ghostInput, previousGhostInput);
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

void GameManager::SwitchRoles()
{
	_gameData.SwitchPlayerAndGhost();
	_playerRole = _playerRole == PlayerRole::PLAYER ? PlayerRole::GHOST : PlayerRole::PLAYER;
	_gameData.SetPlayersRole(_playerRole == PlayerRole::PLAYER);
}
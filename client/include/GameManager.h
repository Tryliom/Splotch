#pragma once

#include "Constants.h"
#include "Packet.h"
#include "PlayerInputs.h"
#include "ClientGameData.h"
#include "ContactListener.h"

#include <queue>

class GameManager final : public Physics::ContactListener
{
 public:
	GameManager(ScreenSizeValue width, ScreenSizeValue height);

 private:
	ClientGameData _gameData;

	PlayerRole _playerRole = PlayerRole::PLAYER;

	ScreenSizeValue _width;
	ScreenSizeValue _height;

 public:
	void OnPacketReceived(Packet& packet);

	PlayerRole GetPlayerRole();

	void Update(sf::Time elapsed, PlayerInput playerInput, PlayerInput previousPlayerInput,
		PlayerInput handInput, PlayerInput previousHandInput);
	void UpdatePlayerAnimations(sf::Time elapsed, sf::Time elapsedSinceLastFixed);

	[[nodiscard]] ClientGameData GetGameData() const;
	void SetGameData(ClientGameData gameData);

	void OnTriggerEnter(Physics::ColliderRef colliderRef, Physics::ColliderRef otherColliderRef) noexcept override {}
	void OnTriggerExit(Physics::ColliderRef colliderRef, Physics::ColliderRef otherColliderRef) noexcept override {}
	void OnTriggerStay(Physics::ColliderRef colliderRef, Physics::ColliderRef otherColliderRef) noexcept override {}

	void OnCollisionEnter(Physics::ColliderRef colliderRef, Physics::ColliderRef otherColliderRef) noexcept override {}
	void OnCollisionExit(Physics::ColliderRef colliderRef, Physics::ColliderRef otherColliderRef) noexcept override {}
	void OnCollisionStay(Physics::ColliderRef colliderRef, Physics::ColliderRef otherColliderRef) noexcept override {}
};
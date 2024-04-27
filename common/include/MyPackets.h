#pragma once

#include "Packet.h"
#include "PacketManager.h"

namespace MyPackets
{
	void RegisterMyPackets();

	enum class MyPacketType : char
	{
		JoinLobby = static_cast<char>(PacketType::COUNT), // All custom packets should be after COUNT
		LeaveLobby,
		LeaveGame,
		StartGame,
		PlayerInput,
		ConfirmationInput,
		SwitchRole,
		COUNT
	};
}
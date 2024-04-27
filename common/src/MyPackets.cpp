#include "MyPackets.h"

#include "PacketManager.h"
#include "MyPackets/JoinLobbyPacket.h"
#include "MyPackets/LeaveLobbyPacket.h"
#include "MyPackets/LeaveGamePacket.h"
#include "MyPackets/StartGamePacket.h"
#include "MyPackets/PlayerInputPacket.h"
#include "MyPackets/ConfirmationInputPacket.h"
#include "MyPackets/SwitchRolePacket.h"

namespace MyPackets
{
	void RegisterMyPackets()
	{
		PacketManager::RegisterPacketType(new JoinLobbyPacket());
		PacketManager::RegisterPacketType(new LeaveLobbyPacket());
		PacketManager::RegisterPacketType(new LeaveGamePacket());
		PacketManager::RegisterPacketType(new StartGamePacket());
		PacketManager::RegisterPacketType(new PlayerInputPacket());
		PacketManager::RegisterPacketType(new ConfirmInputPacket());
		PacketManager::RegisterPacketType(new SwitchRolePacket());
	}
}

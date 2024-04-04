#include "MyPackets.h"

#include "PacketManager.h"
#include "MyPackets/LobbyInformationPacket.h"
#include "MyPackets/JoinLobbyPacket.h"
#include "MyPackets/LeaveLobbyPacket.h"
#include "MyPackets/LeaveGamePacket.h"

namespace MyPackets
{
	void RegisterMyPackets()
	{
		PacketManager::RegisterPacketType(new LobbyInformationPacket());
		PacketManager::RegisterPacketType(new JoinLobbyPacket());
		PacketManager::RegisterPacketType(new LeaveLobbyPacket());
		PacketManager::RegisterPacketType(new LeaveGamePacket());
	}
}

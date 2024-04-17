#include "GameServer.h"
#include "PacketManager.h"
#include "MyPackets.h"

int main()
{
	MyPackets::RegisterMyPackets();

	NetworkServerManager networkServerManager(PORT);
	GameServer server(networkServerManager);

	while(networkServerManager.Running)
	{
		server.Update();
	}

	return EXIT_SUCCESS;
}
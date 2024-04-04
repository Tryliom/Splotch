#include "GameServer.h"
#include "PacketManager.h"
#include "MyPackets.h"
#include "AssetManager.h"
#include "Game.h"
#include "NetworkClientManager.h"
#include "Logger.h"
#include "GameManager.h"

inline static ScreenSizeValue HEIGHT = { 900.f };
inline static ScreenSizeValue WIDTH = { 700.f };

int main()
{
	MyPackets::RegisterMyPackets();
	AssetManager::Initialize();

	NetworkServerManager networkServerManager(PORT);
	Server server(networkServerManager);

	// Network
	std::array<NetworkClientManager, 2> networkClientManagers = {
		NetworkClientManager(HOST_NAME, PORT),
		NetworkClientManager(HOST_NAME, PORT)
	};

	// Set the size of the game
	sf::RenderWindow window(sf::RenderWindow(sf::VideoMode(WIDTH.Value, HEIGHT.Value), "Splotch", sf::Style::Default));

	window.setVerticalSyncEnabled(true);

	std::array<GameManager, 2> gameManagers = {
		GameManager(WIDTH, HEIGHT),
		GameManager(WIDTH, HEIGHT)
	};
	std::array<Game, 2> games = {
		Game(gameManagers[0], networkClientManagers[0], WIDTH, HEIGHT),
		Game(gameManagers[1], networkClientManagers[1], WIDTH, HEIGHT)
	};

	for (auto& game : games)
	{
		game.OnQuit([&]() {
			window.close();
		});
	}

	sf::Clock clock;
	int gameIndex = 0;
	auto gameNumber = Text(
			sf::Vector2f(10, 10),
			{ TextLine({ CustomText{ .Text = "Game " + std::to_string(gameIndex), .Size = 18 }}) }, -1, false
	);

	while (window.isOpen())
	{
		sf::Event event{};
		sf::Time elapsed = clock.restart();

		server.Update();

		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
			{
				window.close();
				break;
			}

			if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Tab)
			{
				gameIndex = (gameIndex + 1) % static_cast<int>(games.size());
				gameNumber = Text(
						sf::Vector2f(10, 10),
						{ TextLine({ CustomText{ .Text = "Game " + std::to_string(gameIndex), .Size = 18 }}) }, -1, false
				);
			}

			games[gameIndex].CheckInputs(event);
		}

		for (auto& game : games)
		{
			game.Update(elapsed, sf::Vector2f(sf::Mouse::getPosition(window)));
		}

		window.clear();
		games[gameIndex].Draw(window);
		window.draw(gameNumber);
		window.display();
	}

	for (auto& networkClientManager : networkClientManagers)
	{
		networkClientManager.Stop();
	}

	networkServerManager.Running = false;

	return EXIT_SUCCESS;
}

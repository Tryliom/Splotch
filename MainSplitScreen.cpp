#include "GameServer.h"
#include "PacketManager.h"
#include "MyPackets.h"
#include "AssetManager.h"
#include "Game.h"
#include "NetworkClientManager.h"
#include "Logger.h"
#include "GameManager.h"

inline static ScreenSizeValue HEIGHT = { 900.f };
inline static ScreenSizeValue WIDTH_PER_SCREEN = { 700.f };
inline static ScreenSizeValue OFFSET_BETWEEN_SCREEN = { 10.f };
inline static ScreenSizeValue WIDTH = WIDTH_PER_SCREEN * 2.f + OFFSET_BETWEEN_SCREEN;

inline static int FRAME_RATE = 30;
inline static float TIME_PER_FRAME = 1.f / FRAME_RATE;

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
		GameManager(WIDTH_PER_SCREEN, HEIGHT),
		GameManager(WIDTH_PER_SCREEN, HEIGHT)
	};
	std::array<Game, 2> games = {
		Game(gameManagers[0], networkClientManagers[0], WIDTH_PER_SCREEN, HEIGHT),
		Game(gameManagers[1], networkClientManagers[1], WIDTH_PER_SCREEN, HEIGHT)
	};

	for (auto& game : games)
	{
		game.OnQuit([&]() {
			window.close();
		});
	}

	sf::Clock clock;
	float time = TIME_PER_FRAME;

	while (window.isOpen())
	{
		sf::Event event{};
		sf::Time elapsed = clock.restart();

		time += elapsed.asSeconds();

		while (time >= TIME_PER_FRAME)
		{
			server.Update();

			while (window.pollEvent(event))
			{
				if (event.type == sf::Event::Closed)
				{
					window.close();
					break;
				}

				for (auto i = 0; i < 2; i++)
				{
					auto& game = games[i];

					// WASD for player 1, arrow keys for player 2
					if (event.type == sf::Event::KeyPressed && i == 1)
					{
						// If key is WASD, break
						if (event.key.code == sf::Keyboard::Key::W || event.key.code == sf::Keyboard::Key::A
							|| event.key.code == sf::Keyboard::Key::S || event.key.code == sf::Keyboard::Key::D)
						{
							break;
						}

						// Set value of arrow key to WASD key
						switch (event.key.code)
						{
						case sf::Keyboard::Key::Up: event.key.code = sf::Keyboard::Key::W; break;
						case sf::Keyboard::Key::Down: event.key.code = sf::Keyboard::Key::S; break;
						case sf::Keyboard::Key::Left: event.key.code = sf::Keyboard::Key::A; break;
						case sf::Keyboard::Key::Right: event.key.code = sf::Keyboard::Key::D; break;
						default: break;
						}
					}

					game.CheckInputs(event);
				}
			}

			for (int i = 0; i < 2; i++)
			{
				auto& game = games[i];

				game.FixedUpdate(sf::seconds(TIME_PER_FRAME));
			}

			time -= TIME_PER_FRAME;
		}

		auto mousePosition = sf::Vector2f(sf::Mouse::getPosition(window));

		for (int i = 0; i < 2; i++)
		{
			auto& game = games[i];

			game.Update(elapsed, mousePosition - sf::Vector2f(i * (WIDTH_PER_SCREEN.Value + OFFSET_BETWEEN_SCREEN.Value), 0));
		}

		window.clear();

		// Create 2 images for each screen
		for (int i = 0; i < 2; i++)
		{
			sf::RenderTexture renderTexture;
			renderTexture.create(WIDTH_PER_SCREEN.Value, HEIGHT.Value);
			renderTexture.clear(sf::Color::Black);

			games[i].Draw(renderTexture);

			sf::Sprite gameView;
			gameView.setPosition(sf::Vector2f(i * (WIDTH_PER_SCREEN.Value + OFFSET_BETWEEN_SCREEN.Value), 0));
			gameView.setTexture(renderTexture.getTexture());
			gameView.setOrigin(sf::Vector2f(0, HEIGHT.Value));
			gameView.setScale(sf::Vector2f(1, -1));

			window.draw(gameView);
		}

		window.display();
	}

	for (auto& networkClientManager : networkClientManagers)
	{
		networkClientManager.Stop();
	}

	networkServerManager.Running = false;

	return EXIT_SUCCESS;
}

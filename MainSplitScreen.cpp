#include "GameServer.h"
#include "PacketManager.h"
#include "MyPackets.h"
#include "AssetManager.h"
#include "Game.h"
#include "NetworkClientManager.h"
#include "Logger.h"
#include "GameManager.h"
#include "RollbackManager.h"

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
	std::array<NetworkClientManager, MAX_PLAYERS> networkClientManagers = {
		NetworkClientManager(HOST_NAME, PORT),
		NetworkClientManager(HOST_NAME, PORT)
	};

	// Set the size of the game
	sf::RenderWindow window(sf::RenderWindow(sf::VideoMode(WIDTH.Value, HEIGHT.Value), "Splotch", sf::Style::Default));

	window.setVerticalSyncEnabled(true);

	std::array<GameManager, MAX_PLAYERS> gameManagers = {
		GameManager(WIDTH_PER_SCREEN, HEIGHT),
		GameManager(WIDTH_PER_SCREEN, HEIGHT)
	};
	std::array<RollbackManager, MAX_PLAYERS> rollbackManagers = {
		RollbackManager(),
		RollbackManager()
	};
	std::array<Game, MAX_PLAYERS> games = {
		Game(rollbackManagers[0], gameManagers[0], networkClientManagers[0], WIDTH_PER_SCREEN, HEIGHT),
		Game(rollbackManagers[1], gameManagers[1], networkClientManagers[1], WIDTH_PER_SCREEN, HEIGHT)
	};

	for (auto& game : games)
	{
		game.OnQuit([&]()
		{
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

				for (auto i = 0; i < MAX_PLAYERS; i++)
				{
					auto& game = games[i];

					game.CheckInputs(event);
				}
			}

			std::array<sf::Keyboard::Key, 4> keys1 = {
				sf::Keyboard::Key::W,
				sf::Keyboard::Key::A,
				sf::Keyboard::Key::S,
				sf::Keyboard::Key::D
			};
			std::array<sf::Keyboard::Key, 4> keys2 = {
				sf::Keyboard::Key::Up,
				sf::Keyboard::Key::Left,
				sf::Keyboard::Key::Down,
				sf::Keyboard::Key::Right
			};

			for (int i = 0; i < MAX_PLAYERS; i++)
			{
				auto& game = games[i];
				PlayerInput playerInput = {};
				PlayerRole playerRole = gameManagers[i].GetPlayerRole();
				std::array<sf::Keyboard::Key, 4> keys = i == 0 ? keys1 : keys2;

				if (playerRole == PlayerRole::PLAYER)
				{
					if (sf::Keyboard::isKeyPressed(keys[1]))
					{
						playerInput |= static_cast<std::uint8_t>(PlayerInputTypes::Left);
					}

					if (sf::Keyboard::isKeyPressed(keys[3]))
					{
						playerInput |= static_cast<std::uint8_t>(PlayerInputTypes::Right);
					}

					if (sf::Keyboard::isKeyPressed(keys[0]))
					{
						playerInput |= static_cast<std::uint8_t>(PlayerInputTypes::Up);
					}
				}
				else
				{
					if (sf::Keyboard::isKeyPressed(keys[2]))
					{
						playerInput |= static_cast<std::uint8_t>(PlayerInputTypes::Down);
					}

					if (sf::Keyboard::isKeyPressed(keys[1]))
					{
						playerInput |= static_cast<std::uint8_t>(PlayerInputTypes::Left);
					}

					if (sf::Keyboard::isKeyPressed(keys[3]))
					{
						playerInput |= static_cast<std::uint8_t>(PlayerInputTypes::Right);
					}
				}

				game.OnPlayerInput(playerInput);
				game.FixedUpdate(sf::seconds(TIME_PER_FRAME));
			}

			time -= TIME_PER_FRAME;
		}

		auto mousePosition = sf::Vector2f(sf::Mouse::getPosition(window));

		for (int i = 0; i < MAX_PLAYERS; i++)
		{
			auto& game = games[i];

			game.Update(elapsed, mousePosition - sf::Vector2f(i * (WIDTH_PER_SCREEN.Value + OFFSET_BETWEEN_SCREEN.Value), 0));
		}

		window.clear();

		// Create 2 images for each screen
		for (int i = 0; i < MAX_PLAYERS; i++)
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

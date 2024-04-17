#include "GameServer.h"
#include "PacketManager.h"
#include "MyPackets.h"
#include "AssetManager.h"
#include "Game.h"
#include "NetworkClientManager.h"
#include "Logger.h"
#include "GameManager.h"
#include "RollbackManager.h"

#include <imgui.h>
#include <imgui-SFML.h>

struct ClientNetworkSettings
{
	float ChanceToDropPacket = 0.f;
	float MinLatency = 0.f;
	float MaxLatency = 0.f;
};

enum class NetworkSettings
{
	LOCAL,
	NORMAL,
	BAD,
	WORST
};

constexpr std::string ToString(NetworkSettings networkSettings)
{
	switch (networkSettings)
	{
	case NetworkSettings::LOCAL:
		return "Local";
	case NetworkSettings::NORMAL:
		return "Normal";
	case NetworkSettings::BAD:
		return "Bad";
	case NetworkSettings::WORST:
		return "Worst";
	default:
		return "Unknown";
	}
}

constexpr ClientNetworkSettings LOCAL_SETTINGS = { 0.f, 0.f, 0.f };
constexpr ClientNetworkSettings NORMAL_SETTINGS = { 0.05f, 0.05f, 0.1f };
constexpr ClientNetworkSettings BAD_SETTINGS = { 0.1f, 0.1f, 0.3f };
constexpr ClientNetworkSettings WORST_SETTINGS = { 0.2f, 0.2f, 0.5f };
constexpr std::array<ClientNetworkSettings, 4> NETWORK_SETTINGS = {
	LOCAL_SETTINGS,
	NORMAL_SETTINGS,
	BAD_SETTINGS,
	WORST_SETTINGS
};

constexpr ScreenSizeValue HEIGHT = { 900.f };
constexpr ScreenSizeValue WIDTH_PER_SCREEN = { 700.f };
constexpr ScreenSizeValue OFFSET_BETWEEN_SCREEN = { 10.f };
constexpr ScreenSizeValue WIDTH = WIDTH_PER_SCREEN * 2.f + OFFSET_BETWEEN_SCREEN;
constexpr float GAME_WIDTH = WIDTH_PER_SCREEN.Value;
constexpr float GAME_HEIGHT = HEIGHT.Value;

constexpr int FRAME_RATE = 30;
constexpr float TIME_PER_FRAME = 1.f / FRAME_RATE;

constexpr std::array<sf::Keyboard::Key, 4> player1Commands = {
	sf::Keyboard::Key::W,
	sf::Keyboard::Key::A,
	sf::Keyboard::Key::S,
	sf::Keyboard::Key::D
};
constexpr std::array<sf::Keyboard::Key, 4> player2Commands = {
	sf::Keyboard::Key::Up,
	sf::Keyboard::Key::Left,
	sf::Keyboard::Key::Down,
	sf::Keyboard::Key::Right
};

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
	std::array<std::size_t, MAX_PLAYERS> clientNetworkSettingsSelected = {
		static_cast<std::size_t>(NetworkSettings::NORMAL),
		static_cast<std::size_t>(NetworkSettings::NORMAL)
	};

	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		const auto& clientNetworkSettings = NETWORK_SETTINGS[clientNetworkSettingsSelected[i]];
		networkClientManagers[i].SetDelaySettings(clientNetworkSettings.ChanceToDropPacket,
			clientNetworkSettings.MinLatency,
			clientNetworkSettings.MaxLatency);
	}

	// Set the size of the game
	sf::RenderWindow window(sf::RenderWindow(sf::VideoMode(GAME_WIDTH, GAME_HEIGHT),
	"Splotch", sf::Style::Default));

	window.setVerticalSyncEnabled(true);

	ImGui::SFML::Init(window);

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
				ImGui::SFML::ProcessEvent(event);

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

			for (int i = 0; i < MAX_PLAYERS; i++)
			{
				auto& game = games[i];

				if (game.GetState() == GameState::GAME)
				{
					PlayerInput playerInput = {};
					PlayerRole playerRole = gameManagers[i].GetPlayerRole();
					std::array<sf::Keyboard::Key, 4> keys = i == 0 ? player1Commands : player2Commands;

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

					game.RegisterPlayerInput(playerInput);
				}

				game.FixedUpdate(sf::seconds(TIME_PER_FRAME));
			}

			// If one is in game, then it means both are in game
			if (games[0].GetState() == GameState::GAME)
			{
				if (rollbackManagers[0].GetConfirmedFrame() == rollbackManagers[1].GetConfirmedFrame())
				{
					if (rollbackManagers[0].GetConfirmedGameData() != rollbackManagers[1].GetConfirmedGameData())
					{
						LOG("Game data is different");
					}
				}
			}

			time -= TIME_PER_FRAME;
		}

		auto mousePosition = sf::Vector2f(sf::Mouse::getPosition(window));
		auto timeSinceLastFixed = sf::seconds(time);

		for (int i = 0; i < MAX_PLAYERS; i++)
		{
			auto& game = games[i];

			if (i == 1)
			{
				mousePosition.x -= WIDTH_PER_SCREEN.Value + OFFSET_BETWEEN_SCREEN.Value;
			}

			game.Update(elapsed, timeSinceLastFixed, mousePosition);
		}

		ImGui::SFML::Update(window, elapsed);

		{
			ImGui::Begin("Network Settings");

			for (int i = 0; i < MAX_PLAYERS; i++)
			{
				const auto settings = clientNetworkSettingsSelected[i];

				ImGui::Text("Player %d: %s", i + 1, ToString(static_cast<NetworkSettings>(settings)).c_str());

				for (int setting = 0; setting < NETWORK_SETTINGS.size(); setting++)
				{
					const auto str = "P" + std::to_string(i + 1) + " " + ToString(static_cast<NetworkSettings>(setting));

					if (ImGui::Button(str.c_str()))
					{
						clientNetworkSettingsSelected[i] = setting;
						auto clientNetworkSettings = NETWORK_SETTINGS[setting];
						networkClientManagers[i].SetDelaySettings(clientNetworkSettings.ChanceToDropPacket,
							clientNetworkSettings.MinLatency,
							clientNetworkSettings.MaxLatency);
					}

					if (setting < NETWORK_SETTINGS.size() - 1)
					{
						ImGui::SameLine();
					}
				}
			}
		}

		ImGui::End();

		window.clear();

		// Create 2 images for each screen
		for (int i = 0; i < MAX_PLAYERS; i++)
		{
			sf::RenderTexture renderTexture;
			renderTexture.create(GAME_WIDTH, GAME_HEIGHT);
			renderTexture.clear(sf::Color::Black);

			games[i].Draw(renderTexture);

			sf::Sprite gameView;
			sf::Vector2f position = { 0, 0 };

			if (i == 1) position.x = WIDTH_PER_SCREEN.Value + OFFSET_BETWEEN_SCREEN.Value;

			gameView.setPosition(position);
			gameView.setTexture(renderTexture.getTexture());
			gameView.setOrigin(sf::Vector2f(0, HEIGHT.Value));
			gameView.setScale(sf::Vector2f(1, -1));

			window.draw(gameView);
		}

		ImGui::SFML::Render(window);

		window.display();
	}

	for (auto& networkClientManager : networkClientManagers)
	{
		networkClientManager.Stop();
	}

	networkServerManager.Running = false;

	return EXIT_SUCCESS;
}

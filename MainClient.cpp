#include "AssetManager.h"
#include "Application.h"
#include "MyPackets.h"
#include "NetworkClientManager.h"
#include "GameManager.h"

constexpr ScreenSizeValue HEIGHT = { 900.f };
constexpr ScreenSizeValue WIDTH = { 700.f };

constexpr float GAME_WIDTH = WIDTH.Value;
constexpr float GAME_HEIGHT = HEIGHT.Value;

constexpr std::array<sf::Keyboard::Key, 4> commands = {
	sf::Keyboard::Key::W,
	sf::Keyboard::Key::A,
	sf::Keyboard::Key::S,
	sf::Keyboard::Key::D
};

int main()
{
	MyPackets::RegisterMyPackets();
	AssetManager::Initialize();

	// Network
	NetworkClientManager networkClientManager(HOST_NAME, PORT);

	// Set the size of the application
	sf::RenderWindow window(sf::RenderWindow(sf::VideoMode(GAME_WIDTH, GAME_HEIGHT),"Splotch", sf::Style::Default));

	window.setVerticalSyncEnabled(true);

	GameManager gameManager(WIDTH, HEIGHT);
	RollbackManager rollbackManager;
	Application application(rollbackManager, gameManager, networkClientManager, WIDTH, HEIGHT);

	sf::Clock clock;
	float time = FIXED_TIME_STEP;

	while (application.IsRunning())
	{
		sf::Event event{};
		const sf::Time elapsed = clock.restart();

		time += elapsed.asSeconds();

		while (time >= FIXED_TIME_STEP)
		{
			while (window.pollEvent(event))
			{
				if (event.type == sf::Event::Closed)
				{
					window.close();
					break;
				}

				application.OnInput(event);
			}

			PlayerInput playerInput = {};

			if (sf::Keyboard::isKeyPressed(commands[0]))
			{
				playerInput |= static_cast<std::uint8_t>(PlayerInputTypes::Up);
			}

			if (sf::Keyboard::isKeyPressed(commands[1]))
			{
				playerInput |= static_cast<std::uint8_t>(PlayerInputTypes::Left);
			}

			if (sf::Keyboard::isKeyPressed(commands[2]))
			{
				playerInput |= static_cast<std::uint8_t>(PlayerInputTypes::Down);
			}

			if (sf::Keyboard::isKeyPressed(commands[3]))
			{
				playerInput |= static_cast<std::uint8_t>(PlayerInputTypes::Right);
			}

			application.AddLocalPlayerInput(playerInput);
			application.FixedUpdate();

			time -= FIXED_TIME_STEP;
		}

		const auto mousePosition = sf::Vector2f(sf::Mouse::getPosition(window));
		const auto timeSinceLastFixed = sf::seconds(time);

		application.Update(elapsed, timeSinceLastFixed, mousePosition);

		window.clear();
		application.Draw(window);
		window.display();
	}

	window.close();
	networkClientManager.Stop();

	return EXIT_SUCCESS;
}
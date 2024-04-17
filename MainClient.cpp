#include "AssetManager.h"
#include "Game.h"
#include "MyPackets.h"
#include "NetworkClientManager.h"
#include "GameManager.h"

constexpr ScreenSizeValue HEIGHT = { 900.f };
constexpr ScreenSizeValue WIDTH = { 700.f };

constexpr float GAME_WIDTH = WIDTH.Value;
constexpr float GAME_HEIGHT = HEIGHT.Value;

constexpr int FRAME_RATE = 30;
constexpr float TIME_PER_FRAME = 1.f / FRAME_RATE;

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
	NetworkClientManager _networkClientManager(HOST_NAME, PORT);

	// Set the size of the game
	sf::RenderWindow window(sf::RenderWindow(sf::VideoMode(GAME_WIDTH, GAME_HEIGHT),
	"Splotch", sf::Style::Default));

	window.setVerticalSyncEnabled(true);

	GameManager gameManager(WIDTH, HEIGHT);
	RollbackManager rollbackManager;
	Game game(rollbackManager, gameManager, _networkClientManager, WIDTH, HEIGHT);

	game.OnQuit([&]()
	{
	  	window.close();
	});

	sf::Clock clock;
	float time = TIME_PER_FRAME;

	while (window.isOpen())
	{
		sf::Event event{};
		const sf::Time elapsed = clock.restart();

		time += elapsed.asSeconds();

		while (time >= TIME_PER_FRAME)
		{
			while (window.pollEvent(event))
			{
				if (event.type == sf::Event::Closed)
				{
					window.close();
					break;
				}

				game.CheckInputs(event);
			}

			PlayerInput playerInput = {};

			if (game.GetState() == GameState::GAME)
			{
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

				game.RegisterPlayerInput(playerInput);
			}

			game.FixedUpdate(sf::seconds(TIME_PER_FRAME));

			time -= TIME_PER_FRAME;
		}

		const auto mousePosition = sf::Vector2f(sf::Mouse::getPosition(window));
		const auto timeSinceLastFixed = sf::seconds(time);

		game.Update(elapsed, timeSinceLastFixed, mousePosition);

		window.clear();
		game.Draw(window);
		window.display();
	}

	_networkClientManager.Stop();

	return EXIT_SUCCESS;
}
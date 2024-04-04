#include "AssetManager.h"
#include "Game.h"
#include "MyPackets.h"
#include "NetworkClientManager.h"
#include "Logger.h"
#include "GameManager.h"

inline static ScreenSizeValue HEIGHT = { 900.f };
inline static ScreenSizeValue WIDTH = { 700.f };

int main()
{
	MyPackets::RegisterMyPackets();
	AssetManager::Initialize();

	// Network
	NetworkClientManager _networkClientManager(HOST_NAME, PORT);

	// Set the size of the game
	sf::RenderWindow _window(sf::RenderWindow(sf::VideoMode(WIDTH.Value, HEIGHT.Value), "Splotch", sf::Style::Default));

	_window.setVerticalSyncEnabled(true);

	GameManager _gameManager(WIDTH, HEIGHT);
	Game _game(_gameManager, _networkClientManager, WIDTH, HEIGHT);

	_game.OnQuit([&]() {
		_window.close();
	});

	sf::Clock clock;

	while (_window.isOpen())
	{
		sf::Event event{};

		while (_window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
			{
				_window.close();
				break;
			}

			_game.CheckInputs(event);
		}

		sf::Time elapsed = clock.restart();

		_game.Update(elapsed, sf::Vector2f(sf::Mouse::getPosition(_window)));
		_window.clear();
		_game.Draw(_window);
		_window.display();
	}

	_networkClientManager.Stop();

	return EXIT_SUCCESS;
}
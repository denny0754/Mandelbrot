#include <iostream>
#include "Config.hpp"
#include "MandelbrotUtils.hpp"
#include "Logger.hpp"

#include <SFML/Graphics.hpp>

int main()
{
	Logger::Init("MANDELBROT");

	Mandelbrot::Init();
	// -0.6140625273462111 + -0.40633146872742876i at zoom 3.9041710026e+06

	double zoom = 0.004;
	double offsetX = -0.7;
	double offsetY = 0.0;
	Mandelbrot::SetZoom(zoom);
	Mandelbrot::SetOffset({ offsetX, offsetY });
	Mandelbrot::UseVertexBuffer(true); // TODO Using the Sprite crashes the application. Probably caused by the multi-threaded function. TOFIX<->IMPORTANT
	Mandelbrot::SetMaxIterations(1000u);
	Mandelbrot::ProcessMt();

	sf::ContextSettings settings;
	settings.antialiasingLevel = 16;
	settings.sRgbCapable = true;

	sf::RenderWindow window = sf::RenderWindow(
		sf::VideoMode(Mandelbrot::Config::WINDOW_WIDTH, Mandelbrot::Config::WINDOW_HEIGHT),
		"Mandelbrot Set - by Vaccher Diego",
		sf::Style::Close | sf::Style::Titlebar,
		settings
	);

	while (window.isOpen())
	{
		sf::Event events;
		while (window.pollEvent(events))
		{
			switch (events.type)
			{
				case sf::Event::Closed:
					window.close();
					break;
			}
		}

		window.clear();
		Mandelbrot::DrawMandelbrotSet(window);
		window.display();
	}

	return 0;
}
#include <iostream>
#include "Config.hpp"
#include "MandelbrotUtils.hpp"
#include "MandelbrotGui.hpp"
#include "Logger.hpp"
#include "Timer.hpp"

#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>

void RendererThread(sf::RenderWindow* window)
{
	window->setActive(true);

	while (window->isOpen())
	{
		window->clear();

		Mandelbrot::DrawMandelbrotSet(*window);
		Mandelbrot::Gui::DrawGui(*window);

		window->display();
	}
}

int main()
{
	Logger::Init("MANDELBROT");

	Mandelbrot::Init();
	Mandelbrot::Gui::InitGui();
	// -0.6140625273462111 + -0.40633146872742876i at zoom 3.9041710026e+06 => Nice Zoom

	double zoom = 0.004;
	double offsetX = -0.7;
	double offsetY = 0.0;
	Mandelbrot::SetZoom(zoom);
	Mandelbrot::SetOffset({ offsetX, offsetY });
	Mandelbrot::UseVertexBuffer(false);
	Mandelbrot::SetMaxIterations(1000u);
	Mandelbrot::SetMaxThreads(8);

	Timer timer;
	timer.start();
	Mandelbrot::ProcessMt();
	timer.stop();
	std::cout << timer.elapsedMilliseconds() << std::endl;

	sf::ContextSettings settings;
	settings.antialiasingLevel = 16;
	settings.sRgbCapable = true;

	sf::RenderWindow window = sf::RenderWindow(
		sf::VideoMode(Mandelbrot::Config::WINDOW_WIDTH, Mandelbrot::Config::WINDOW_HEIGHT),
		"Mandelbrot Set - by Vaccher Diego",
		sf::Style::Close | sf::Style::Titlebar,
		settings
	);

	window.setActive(false);

	sf::Thread renderer_thread(&RendererThread, &window);
	renderer_thread.launch();

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
		// Updating the Mandelbrot GUI 
		Mandelbrot::Gui::UpdateGui(window);
	}

	return 0;
}
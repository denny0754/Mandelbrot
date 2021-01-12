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
	Logger::GetLogger()->trace("Size of `double`: {}", sizeof(double));
	Logger::GetLogger()->trace("Size of `long double`: {}", sizeof(long double));

	Mandelbrot::Init();
	Mandelbrot::Gui::InitGui();
	// -0.6140625273462111 + -0.40633146872742876i at zoom 3.9041710026e+06 => Nice Zoom

	long double zoom = 0.004;
	long double offsetX = -0.7;
	long double offsetY = 0.0;
	Mandelbrot::SetZoom(zoom);
	Mandelbrot::SetOffset({ offsetX, offsetY });
	Mandelbrot::UseVertexBuffer(false);
	Mandelbrot::SetMaxIterations(1000u);
	Mandelbrot::SetMaxThreads(8);

	Mandelbrot::SetDefaultZoom(zoom);
	Mandelbrot::SetDefaultOffset({ offsetX, offsetY });
	Mandelbrot::SetDefaultMaxIterations(1000u);

	

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

		if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left) && sf::Keyboard::isKeyPressed(sf::Keyboard::LControl))
		{
			auto pos = sf::Mouse::getPosition();

			auto offset = Mandelbrot::ScaleToPlane({ static_cast<long double>(pos.x), static_cast<long double>(pos.y) });
			Mandelbrot::SetOffset(offset);
			Mandelbrot::Update();
		}
	}

	return 0;
}
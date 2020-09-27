#include <iostream>
#include "Config.hpp"
#include "MandelbrotUtils.hpp"
#include "Logger.hpp"
#include "Button.hpp"
#include "Timer.hpp"

#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>

struct MandelbrotGuiElements
{
	static inline sf::RectangleShape yaxis_line = sf::RectangleShape({ 2.f , Mandelbrot::Config::WINDOW_HEIGHT });
	static inline sf::RectangleShape xaxis_line = sf::RectangleShape({ Mandelbrot::Config::WINDOW_WIDTH , 2.f });

	static inline Mandelbrot::Gui::Button ZoomInButton = Mandelbrot::Gui::Button({ 100, 25 }, { 0, 0 }, "ZOOM-IN");
	static inline Mandelbrot::Gui::Button ZoomOutButton = Mandelbrot::Gui::Button({ 100, 25 }, { 100, 0 }, "ZOOM-OUT");

	static inline Mandelbrot::Gui::Button OffsetXPlusButton = Mandelbrot::Gui::Button({ 100, 25 }, { 200, 0 }, "OFFX+");
	static inline Mandelbrot::Gui::Button OffsetXMinusButton = Mandelbrot::Gui::Button({ 100, 25 }, { 300, 0 }, "OFFX-");

	static inline Mandelbrot::Gui::Button OffsetYPlusButton = Mandelbrot::Gui::Button({ 100, 25 }, { 400, 0 }, "OFFY+");
	static inline Mandelbrot::Gui::Button OffsetYMinusButton = Mandelbrot::Gui::Button({ 100, 25 }, { 500, 0 }, "OFFY-");

	static inline Mandelbrot::Gui::Button IterationsPlusButton = Mandelbrot::Gui::Button({ 100, 25 }, { 600, 0 }, "ITER+");
	static inline Mandelbrot::Gui::Button IterationsMinusButton = Mandelbrot::Gui::Button({ 100, 25 }, { 700, 0 }, "ITER-");
};

void RendererThread(sf::RenderWindow* window)
{
	window->setActive(true);

	while (window->isOpen())
	{
		window->clear();

		Mandelbrot::DrawMandelbrotSet(*window);
		window->draw(MandelbrotGuiElements::xaxis_line);
		window->draw(MandelbrotGuiElements::yaxis_line);
		window->draw(MandelbrotGuiElements::ZoomInButton);
		window->draw(MandelbrotGuiElements::ZoomOutButton);
		window->draw(MandelbrotGuiElements::OffsetXPlusButton);
		window->draw(MandelbrotGuiElements::OffsetXMinusButton);
		window->draw(MandelbrotGuiElements::OffsetYPlusButton);
		window->draw(MandelbrotGuiElements::OffsetYMinusButton);
		window->draw(MandelbrotGuiElements::IterationsPlusButton);
		window->draw(MandelbrotGuiElements::IterationsMinusButton);

		window->display();
	}
}

int main()
{
	Logger::Init("MANDELBROT");

	Mandelbrot::Init();
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

	MandelbrotGuiElements::xaxis_line.setFillColor(sf::Color(255, 255, 255, 100));
	MandelbrotGuiElements::xaxis_line.setPosition({ 0, Mandelbrot::Config::WINDOW_HEIGHT / 2.f });
	
	MandelbrotGuiElements::yaxis_line.setFillColor(sf::Color(255, 255, 255, 100));
	MandelbrotGuiElements::yaxis_line.setPosition({ Mandelbrot::Config::WINDOW_WIDTH / 2.f,  0 });

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

		MandelbrotGuiElements::ZoomInButton.Update(window);
		MandelbrotGuiElements::ZoomOutButton.Update(window);
		MandelbrotGuiElements::OffsetXPlusButton.Update(window);
		MandelbrotGuiElements::OffsetXMinusButton.Update(window);
		MandelbrotGuiElements::OffsetYPlusButton.Update(window);
		MandelbrotGuiElements::OffsetYMinusButton.Update(window);
		MandelbrotGuiElements::IterationsPlusButton.Update(window);
		MandelbrotGuiElements::IterationsMinusButton.Update(window);

		// TODO Code below MUST be cleaned up. Either move button events on a function or create a `OnButtonPress` method inside the button class.
		if (MandelbrotGuiElements::OffsetXPlusButton.GetCurrentState() == Mandelbrot::Gui::Button::ButtonState::Pressed)
		{
			auto offset = Mandelbrot::GetOffset();
			Mandelbrot::SetOffset(
				{
					offset.x + 0.0008,
					offset.y
				}
			);
			sf::Thread process_thread(&Mandelbrot::ProcessMt);
			process_thread.launch();
		}
		else if (MandelbrotGuiElements::OffsetXMinusButton.GetCurrentState() == Mandelbrot::Gui::Button::ButtonState::Pressed)
		{
			auto offset = Mandelbrot::GetOffset();
			Mandelbrot::SetOffset(
				{
					offset.x - 0.0008,
					offset.y
				}
			);
			sf::Thread process_thread(&Mandelbrot::ProcessMt);
			process_thread.launch();
		}

		if (MandelbrotGuiElements::OffsetYPlusButton.GetCurrentState() == Mandelbrot::Gui::Button::ButtonState::Pressed)
		{
			auto offset = Mandelbrot::GetOffset();
			Mandelbrot::SetOffset(
				{
					offset.x,
					offset.y + 0.0008
				}
			);
			sf::Thread process_thread(&Mandelbrot::ProcessMt);
			process_thread.launch();
		}
		else if (MandelbrotGuiElements::OffsetYMinusButton.GetCurrentState() == Mandelbrot::Gui::Button::ButtonState::Pressed)
		{
			auto offset = Mandelbrot::GetOffset();
			Mandelbrot::SetOffset(
				{
					offset.x,
					offset.y - 0.0008
				}
			);
			sf::Thread process_thread(&Mandelbrot::ProcessMt);
			process_thread.launch();
		}

		if (MandelbrotGuiElements::ZoomInButton.GetCurrentState() == Mandelbrot::Gui::Button::ButtonState::Pressed)
		{
			auto lzoom = Mandelbrot::GetZoom();
			Mandelbrot::SetZoom(lzoom - 0.0004);
			sf::Thread process_thread(&Mandelbrot::ProcessMt);
			process_thread.launch();
		}
		else if (MandelbrotGuiElements::ZoomOutButton.GetCurrentState() == Mandelbrot::Gui::Button::ButtonState::Pressed)
		{
			auto lzoom = Mandelbrot::GetZoom();
			Mandelbrot::SetZoom(lzoom + 0.0004);
			sf::Thread process_thread(&Mandelbrot::ProcessMt);
			process_thread.launch();
		}

		if (MandelbrotGuiElements::IterationsPlusButton.GetCurrentState() == Mandelbrot::Gui::Button::ButtonState::Pressed)
		{
			auto iters = Mandelbrot::GetMaxIterations();
			Mandelbrot::SetMaxIterations(iters + 1000);
			sf::Thread process_thread(&Mandelbrot::ProcessMt);
			process_thread.launch();
		}
		else if (MandelbrotGuiElements::IterationsMinusButton.GetCurrentState() == Mandelbrot::Gui::Button::ButtonState::Pressed)
		{
			auto iters = Mandelbrot::GetMaxIterations();
			Mandelbrot::SetMaxIterations(iters - 1000);
			sf::Thread process_thread(&Mandelbrot::ProcessMt);
			process_thread.launch();
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::R))
		{
			Mandelbrot::SetZoom(zoom);
			Mandelbrot::SetOffset({ offsetX, offsetY });
			Mandelbrot::UseVertexBuffer(false);
			Mandelbrot::SetMaxIterations(1000u);
			Mandelbrot::SetMaxThreads(8);
			sf::Thread process_thread(&Mandelbrot::ProcessMt);
			process_thread.launch();
		}
	}

	return 0;
}
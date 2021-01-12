#pragma once
#ifndef MANDELBROT_MANDELBROTGUI_HPP
#define MANDELBROT_MANDELBROTGUI_HPP

#include <SFML/Graphics.hpp>

namespace Mandelbrot
{
	namespace Gui
	{
		void InitGui();

		void UpdateGui(sf::RenderWindow& window);

		void DrawGui(sf::RenderWindow& window);

		void HideGui();

		void ShowGui();
	}
}

#endif
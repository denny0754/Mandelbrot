#pragma once
#ifndef MANDELBROT_MANDELBROTPOINT_HPP
#define MANDELBROT_MANDELBROTPOINT_HPP

#include <SFML/Graphics/Color.hpp>

namespace Mandelbrot
{
	struct MandelbrotPoint
	{
		// Position x and y on the screen.
		int ScreenX{ 0 };
		int ScreenY{ 0 };

		// True coordinates on the real/imaginary plane.
		double Real{ .0 };
		double Imaginary{ .0 };

		// Color of the point.
		sf::Color Color{ 0, 0, 0, 255 };
	};
}

#endif
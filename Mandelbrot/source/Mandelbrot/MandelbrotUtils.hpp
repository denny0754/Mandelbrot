#pragma once
#ifndef MANDELBROT_MANDELBROTUTILS_HPP
#define MANDELBROT_MANDELBROTUTILS_HPP

#include <iostream>
#include <SFML/Graphics.hpp>

namespace sf
{
	typedef Vector2<long double>  Vector2ld;
}

namespace Mandelbrot
{
	// Initializes the Mandelbrot set
	void Init();

	// Process Mandelbrot points in Single-threaded Mode
	void ProcessSt();

	// Process Mandelbrot points in Multi-threaded Mode
	void ProcessMt();

	std::size_t GetPointIterations(const sf::Vector2ld& plane_coords);

	// Returns the true x-y coordinates of the Set.
	sf::Vector2ld ScaleToPlane(const sf::Vector2ld& coords);

	void SetMaxIterations(std::size_t iter);
	std::size_t GetMaxIterations();
	void SetDefaultMaxIterations(std::size_t iter);
	std::size_t GetDefaultMaxIterations();

	sf::Color GetPointColor(std::size_t iter);

	// Multi Thread Functions
	void SetMaxThreads(std::size_t threads);
	std::size_t GetMaxThreads();

	// Use vertex buffer instead of the sprite.
	// Note: If VertexBuffer is not avaiable on the system, it won't be used
	// even if `enable` is set to true.
	void UseVertexBuffer(bool enable = sf::VertexBuffer::isAvailable());
	bool IsUsingVertexBuffer();

	void DrawMandelbrotSet(sf::RenderWindow& renderer);

	void SetZoom(const long double& zoom);
	long double GetZoom();
	void SetDefaultZoom(long double zoom);
	long double GetDefaultZoom();

	void SetOffset(const sf::Vector2ld& offset);
	sf::Vector2ld GetOffset();
	void SetDefaultOffset(const sf::Vector2ld& offset);
	sf::Vector2ld GetDefaultOffset();

	// Updates the plane(vertex buffer or sprite) only if something has changed.
	void Update();
	// Updates the plane(vertex buffer or sprite).
	void ForceUpdate();
}

#endif
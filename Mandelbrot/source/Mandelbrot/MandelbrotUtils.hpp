#pragma once
#ifndef MANDELBROT_MANDELBROTUTILS_HPP
#define MANDELBROT_MANDELBROTUTILS_HPP

#include <iostream>
#include <SFML/Graphics.hpp>

namespace sf
{
	typedef Vector2<double>  Vector2d;
}

namespace Mandelbrot
{
	// Initializes the Mandelbrot set
	void Init();

	// Process Mandelbrot points in Single-threaded Mode
	void ProcessSt();

	// Process Mandelbrot points in Multi-threaded Mode
	void ProcessMt();

	std::size_t GetPointIterations(const sf::Vector2d& plane_coords);

	void SetMaxIterations(std::size_t iter);
	std::size_t GetMaxIterations();

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

	void SetZoom(const double& zoom);
	double GetZoom();

	void SetOffset(const sf::Vector2d& offset);
	sf::Vector2d GetOffset();

	// Updates the plane(vertex buffer or sprite) only if something has changed.
	void Update();
	// Updates the plane(vertex buffer or sprite).
	void ForceUpdate();
}

#endif